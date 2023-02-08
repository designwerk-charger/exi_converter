#include "json_parser.h"


/* Static local function */
inline void jp_check_control_char(const std::string & input_data, std::size_t * current_pos, char expected_char);
inline char jp_get_control_char(const std::string & input_data, std::size_t * current_pos);

std::string get_input_data_with_position_marked(std::string input_data, std::size_t current_pos);

inline std::string jp_extract_string(const std::string & input_data, std::size_t * current_pos);
inline std::string jp_extract_value_string(const std::string & input_data, std::size_t * current_pos);
inline std::string jp_extract_value_int(const std::string & input_data, std::size_t * current_pos);

std::shared_ptr<JArray> jp_extract_array(const std::string & input_data, std::size_t * current_pos);
std::pair<std::string, std::shared_ptr<JItemInterface>> jp_extract_single_object(const std::string & input_data,
                                                                                std::size_t * current_pos);
std::shared_ptr<JObject> jp_extract_object(const std::string & input_data, std::size_t * current_pos);




JsonParser::JsonParser(const std::string & input_data) : parsed_json(parse(input_data)) { }

JsonParser::JsonParser() { }

std::shared_ptr<JObject> JsonParser::parse(const std::string & json_str) {
    size_t current_pos = 0;

    auto obj = jp_extract_object(json_str, &current_pos);
    return obj;
}

// an object must start with { and end with }
std::shared_ptr<JObject> jp_extract_object(const std::string & input_data, std::size_t * current_pos) {
    auto o = std::make_shared<JObject>();

    jp_check_control_char(input_data, current_pos, '{');
    char current_char;
    do {
        o->append(jp_extract_single_object(input_data, current_pos));
        current_char = jp_get_control_char(input_data, current_pos);
        if (current_char == ',') {
            (*current_pos)++;
        }
    } while (current_char == ',');

    jp_check_control_char(input_data, current_pos, '}');
    return o;
}

std::pair<std::string, std::shared_ptr<JItemInterface>> jp_extract_single_object(const std::string & input_data,
                                                                                 std::size_t * current_pos) {
    std::pair<std::string, std::shared_ptr<JItemInterface>> o;
    std::string key_str;

    /* get key */
    jp_check_control_char(input_data, current_pos, '"');
    key_str = jp_extract_string(input_data, current_pos);
    jp_check_control_char(input_data, current_pos, ':');


    /* key's value */
    char current_char = jp_get_control_char(input_data, current_pos);
    if (current_char == '"') {
        const auto & value = jp_extract_value_string(input_data, current_pos);
        o = std::make_pair(key_str, std::make_shared<JValue>(value));
    } else if (current_char == '{') {
        size_t next_pos = *current_pos + 1;
        current_char = jp_get_control_char(input_data, &next_pos);

        if (current_char == '}') [[unlikely]] {  // NOLINT
            (*current_pos)++;
            o = std::make_pair(key_str, std::make_shared<JEmpty>());
        } else {
            auto sub_obj = jp_extract_object(input_data, current_pos);
            o = std::make_pair(key_str, sub_obj);
        }
    } else if (current_char == '[') {
        auto value = jp_extract_array(input_data, current_pos);
        o = std::make_pair(key_str, value);
    } else {
        const auto & value = jp_extract_value_int(input_data, current_pos);
        o = std::make_pair(key_str, std::make_shared<JValue>(value));
    }

    return o;
}

// An array must start with [ and must end with ]
std::shared_ptr<JArray> jp_extract_array(const std::string & input_data, std::size_t * current_pos) {
    auto arr = std::make_shared<JArray>();
    jp_check_control_char(input_data, current_pos, '[');

    char current_char = jp_get_control_char(input_data, current_pos);
    if (current_char == '{') {
        do {
            auto obj = jp_extract_object(input_data, current_pos);
            arr->append(obj);
            current_char = jp_get_control_char(input_data, current_pos);
            if (current_char == ',') {
                (*current_pos)++;
            } else {
                break;
            }
        } while (true);
    } else if (current_char == '"') {
        do {
            const auto & value = jp_extract_value_string(input_data, current_pos);
            arr->append(std::make_shared<JValue>(value));
            current_char = jp_get_control_char(input_data, current_pos);
            if (current_char == ',') {
                (*current_pos)++;
            } else {
                break;
            }
        } while (true);
    } else if (current_char == ']') {
        // leave the array empty
    } else {
        throw std::invalid_argument("Unhandled control character '" + std::string(1, current_char)
        + "' at " + std::to_string(*current_pos) + " while parsing array ("
        + get_input_data_with_position_marked(input_data, *current_pos) + ")");
    }

    jp_check_control_char(input_data, current_pos, ']');
    return arr;
}

std::string get_input_data_with_position_marked(std::string input_data, std::size_t current_pos) {
    return input_data.insert(current_pos, "*");
}

inline char jp_get_control_char(const std::string & input_data, std::size_t * current_pos) {
    try {
        char ctrl_char = input_data.at(*current_pos);
        if (ctrl_char == ' ') {
            (*current_pos)++;
            ctrl_char = input_data.at(*current_pos);
        }
        return ctrl_char;
    } catch (const std::out_of_range& _) {
        throw std::invalid_argument("Trying to get character from end (position " + std::to_string(*current_pos)
                                    + ") of string with length " + std::to_string(input_data.size()) + " ("
                                    + get_input_data_with_position_marked(input_data, *current_pos) + ")");
    }
}

inline void jp_check_control_char(const std::string & input_data, std::size_t * current_pos, char expected_char) {
    char current_char = jp_get_control_char(input_data, current_pos);
    if (current_char == expected_char) {
        (*current_pos)++;
    } else {
        throw std::invalid_argument("Unexpected control character '" + std::string(1, current_char)
                                    + "' at position " + std::to_string(*current_pos) + " found! '"
                                    + std::string(1, expected_char) + "' was required! ("
                                    + get_input_data_with_position_marked(input_data, *current_pos) + ")");
    }
}

inline std::string jp_extract_string(const std::string & input_data, std::size_t * current_pos) {
    std::size_t start_str_pos = *current_pos, pos;
    char current_char;
    while ((pos = input_data.find_first_of("\"\\", *current_pos)) != std::string::npos) {
        current_char = input_data.at(pos);
        if (current_char == '\\') {
            // escape character
            *current_pos = pos + 2;
        } else {
            const auto & value = input_data.substr(start_str_pos, pos-start_str_pos);
            *current_pos = pos + 1;
            return value;
        }
    }
    throw std::invalid_argument("something went wrong while extracting string from position "
                                + std::to_string(start_str_pos) + " ("
                                + get_input_data_with_position_marked(input_data, *current_pos) + ")");
}

/* starts with " character */
inline std::string jp_extract_value_string(const std::string & input_data, std::size_t * current_pos) {
    jp_check_control_char(input_data, current_pos, '"');
    auto value_str = jp_extract_string(input_data, current_pos);
    return value_str;
}

/* ends with } or ] or , character */
inline std::string jp_extract_value_int(const std::string & input_data, std::size_t * current_pos) {
    std::size_t pos;
    if ((pos = input_data.find_first_of("}],", *current_pos)) != std::string::npos) {
        const auto &value = input_data.substr(*current_pos, pos - *current_pos);
        *current_pos = pos;
        return value;
    } else {
        throw std::invalid_argument("Error while extracting integer / bool value from " + std::to_string(*current_pos)
                                    + " (" + get_input_data_with_position_marked(input_data, *current_pos) + ")");
    }
}
