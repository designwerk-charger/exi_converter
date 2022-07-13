#include <iostream>
#include <utility>
#include "input_string_stream.h"


InputStringStream::InputStringStream(const std::string & input_data) :
    input_data_(input_data), item_cnt_(0), input_items_(InputStringStream::split_items(input_data)) { }

InputStringStream::InputStringStream() :
        input_data_(), item_cnt_(0), input_items_() { }

void extract_key(const std::string & input_data, std::size_t * current_pos, InputStringStream::item_store_t * data);

std::string input_data_with_position_marked(std::string input_data, std::size_t current_pos) {
    return input_data.insert(current_pos, "*");
}

inline char get_control_char(const std::string & input_data, std::size_t * current_pos) {
    try {
        char ctrl_char = input_data.at(*current_pos);
        if (ctrl_char == ' ') {
            [[unlikely]]
            (*current_pos)++;
            ctrl_char = input_data.at(*current_pos);
        }
        return ctrl_char;
    } catch (const std::out_of_range& _) {
        return '}';
        throw std::invalid_argument("Trying to get character at position " + std::to_string(*current_pos)
                                    + " from string with length " + std::to_string(input_data.size()) + " ("
                                    + input_data_with_position_marked(input_data, *current_pos) + ")");
    }
}

inline void check_control_char(const std::string & input_data, std::size_t * current_pos, char expected_char) {
    char current_char = get_control_char(input_data, current_pos);
    if (current_char == expected_char) {
        (*current_pos)++;
    } else {
        throw std::invalid_argument("Invalid control character '" + std::string(1, current_char)
                                    + "' at position " + std::to_string(*current_pos) + " found! ("
                                    + input_data_with_position_marked(input_data, *current_pos) + ")");
    }
}

inline void check_optional_control_char(const std::string & input_data, std::size_t * current_pos, char expected_char) {
    char current_char = get_control_char(input_data, current_pos);
    if (current_char == expected_char) {
        (*current_pos)++;
    }
}

inline void check_control_chars(const std::string & input_data, std::size_t * current_pos,
                                char expected_char1, char expected_char2) {
    char current_char = get_control_char(input_data, current_pos);
    if ((current_char == expected_char1) || (current_char == expected_char2)) {
        (*current_pos)++;
    } else {
        throw std::invalid_argument("Invalid control characters '" + std::string(1, current_char)
                                    + "' at position " + std::to_string(*current_pos) + " found! ("
                                    + input_data_with_position_marked(input_data, *current_pos) + ")");
    }
}

inline std::string extract_string(const std::string & input_data, std::size_t * current_pos) {
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
    throw std::invalid_argument("somthing went wrong while extracting string from position "
                                + std::to_string(start_str_pos) + " ("
                                + input_data_with_position_marked(input_data, *current_pos) + ")");
}

/* starts with " character */
inline void extract_value_string(const std::string & input_data, std::size_t * current_pos,
                                 InputStringStream::item_store_t * data) {
    check_control_char(input_data, current_pos, '"');
    auto key = extract_string(input_data, current_pos);
    data->emplace_back(std::make_pair(key, InputStringStream::value_str));
}

/* ends with } or ] or , character */
inline void extract_value_int(const std::string & input_data, std::size_t * current_pos,
                              InputStringStream::item_store_t * data) {
    std::size_t pos;
    if ((pos = input_data.find_first_of("}],", *current_pos)) != std::string::npos) {
        const auto &value = input_data.substr(*current_pos, pos - *current_pos);
        *current_pos = pos;
        data->emplace_back(std::make_pair(value, InputStringStream::value_int));
    } else {
        throw std::invalid_argument("Error while extracting integer / bool value from " + std::to_string(*current_pos)
                                    + " (" + input_data_with_position_marked(input_data, *current_pos) + ")");
    }
}

/* a array starts with [ character! */
void extract_array(const std::string & input_data, std::size_t * current_pos, InputStringStream::item_store_t * data) {
    check_control_char(input_data, current_pos, '[');
    data->emplace_back(std::make_pair("[", InputStringStream::arr));

    char current_char = get_control_char(input_data, current_pos);
    if (current_char == '{') {
        do {
            extract_key(input_data, current_pos, data);
            current_char = get_control_char(input_data, current_pos);
            (*current_pos)++;
        } while (current_char == ',');
    } else if (current_char == '"') {
        do {
            extract_value_string(input_data, current_pos, data);
            current_char = get_control_char(input_data, current_pos);
            (*current_pos)++;
        } while (current_char == ',');
    } else {
        throw std::invalid_argument("Unhandled control character '" + std::string(1, current_char)
                                    + "' at " + std::to_string(*current_pos) + " while parsing array ("
                                    + input_data_with_position_marked(input_data, *current_pos) + ")");
    }

    (*current_pos)--;
    check_control_char(input_data, current_pos, ']');
    data->emplace_back(std::make_pair("]", InputStringStream::arr));
}

/* a key starts with {" or ," characters! */
void extract_key(const std::string & input_data, std::size_t * current_pos, InputStringStream::item_store_t * data) {
    check_control_chars(input_data, current_pos, '{', ',');
    char current_char = get_control_char(input_data, current_pos);
    if (current_char == '}') {
        // empty key
        (*current_pos)++;
        return;
    } else if (current_char == '"') {
        (*current_pos)++;
    } else {
        throw std::invalid_argument("Invalid control character at start key '" + std::string(1, current_char)
                                    + "' at position " + std::to_string(*current_pos) + " found! ("
                                    + input_data_with_position_marked(input_data, *current_pos) + ")");
    }

    auto key = extract_string(input_data, current_pos);
    data->emplace_back(std::make_pair(key, InputStringStream::key_str));

    check_control_char(input_data, current_pos, ':');
    current_char = get_control_char(input_data, current_pos);
    if (current_char == '{') {
        do {
            extract_key(input_data, current_pos, data);
            current_char = get_control_char(input_data, current_pos);
        } while (current_char == ',');
    } else if (current_char == '[') {
        extract_array(input_data, current_pos, data);
    } else if (current_char == '"') {
        extract_value_string(input_data, current_pos, data);
    } else {
        extract_value_int(input_data, current_pos, data);
    }

    current_char = get_control_char(input_data, current_pos);
    if (current_char == ',') {
        // next key
        extract_key(input_data, current_pos, data);
    } else if (current_char == '}') {
        // end key
        (*current_pos)++;
    } else if (current_char == ']') {
        // end array, keep position
        return;
    } else {
        throw std::invalid_argument("Unexpected element ending control character '" + std::string(1, current_char)
                                    + "' at " + std::to_string(*current_pos) + " while parsing array ("
                                    + input_data_with_position_marked(input_data, *current_pos) + ")");
    }
}

InputStringStream::item_store_t InputStringStream::split_items(const std::string & input_data) {
    InputStringStream::item_store_t data = {};
    std::size_t current_pos = 0;

    extract_key(input_data, &current_pos, &data);

    return data;
}


void InputStringStream::verify_item_and_move_to_next(const std::string & expected_item) {
    try {
        const std::string &current_item = input_items_.at(item_cnt_++).first;
        if (current_item != expected_item) {
            throw std::runtime_error("Found \"" + current_item + "\" instead of \"" + expected_item
                + "\" item found in " + get_input_data());
        }
    } catch (const std::out_of_range& _) {
        throw std::runtime_error("Tried to verify item \"" + expected_item
                                 + "\" but no further items are available from " + get_input_data());
    }
}

bool InputStringStream::is_list_start_move_to_next() {
    if (input_items_.at(item_cnt_).first == "[") {
        item_cnt_++;
        return true;
    }
    return false;
}

bool InputStringStream::is_list_end_move_to_next() {
    if (input_items_.at(item_cnt_).first == "]") {
        item_cnt_++;
        return true;
    }
    return false;
}

std::string InputStringStream::get_item_and_move_to_next() {
    return input_items_.at(item_cnt_++).first;
}

std::string InputStringStream::get_item() {
    try {
        return input_items_.at(item_cnt_).first;
    } catch (const std::out_of_range& _) {
        return "";
    }
}
