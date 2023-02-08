#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "base_types.h"

BaseTypes::BaseTypes(BitStream * bit_stream) : bit_stream_(bit_stream) {}


void BaseTypes::injectHexBin(const std::string & value, uint8_t max_length) {
    uint8_t length_data_array;
    length_data_array = value.length() >> 1;
    bit_stream_->add_max_8bits(length_data_array, 8);

    uint8_t* data = new uint8_t[length_data_array];

    for (uint8_t byte=0; byte < length_data_array; byte++) {
        data[byte] = std::stoul(value.substr(byte * 2, 2), nullptr, 16);
    }

    bit_stream_->add_bytes(data, length_data_array);
    delete [] data;
}

std::string BaseTypes::extractHexBin(uint8_t max_length) {
    uint8_t length_str = extractIntegerNumber(2, true);

    uint8_t* data = new uint8_t[length_str];
    bit_stream_->get_next_n_bits(length_str*8, data);

    std::ostringstream hexstring;
    for (uint8_t byte=0; byte < length_str; byte++) {
        hexstring << std::setfill('0') << std::setw(2) <<std::uppercase << std::right << std::hex
        << static_cast<int>(data[byte]);
    }

    delete [] data;
    return hexstring.str();
}

uint32_t BaseTypes::extractNBitsForEnum(uint32_t n_bits) {
    uint32_t output_data = bit_stream_->get_max_4bytes(n_bits);
    return output_data;
}

void BaseTypes::injectNBitsForEnum(uint8_t value, uint8_t n_bits) {
    bit_stream_->add_max_8bits(value, n_bits);
}

void BaseTypes::injectString(const std::string & value) {
    bit_stream_->add_max_8bits(value.length() + 2, 8);
    bit_stream_->add_bytes(reinterpret_cast<const uint8_t *>(&value[0]), value.length());
}

std::string BaseTypes::extractString() {
    uint8_t length_str = extractIntegerNumber(2, true);
    if (length_str < 2) {
        throw std::runtime_error("The length of the string is smaller than 2("
        + std::to_string(length_str) + ")");
    }

    length_str -= 2;
    auto out_chars = new uint8_t[length_str + 1];

    for (int i=0; i < length_str; i++) {
        out_chars[i] = bit_stream_->get_max_4bytes(8);
    }

    out_chars[length_str] = 0;

    std::string out_string(reinterpret_cast<const char *>(out_chars));
    delete [] out_chars;
    return out_string;
}

void BaseTypes::injectUri(const std::string & value) {
    injectString(value);
}

std::string BaseTypes::extractUri() {
    return extractString();
}

void BaseTypes::_injectUnsignedNumber(uint32_t number) {
    uint8_t pos_highest_set_bit = 0;
    uint32_t tmp_number;
    tmp_number = number;
    while (tmp_number >>= 1) {
        pos_highest_set_bit++;
    }

    while (pos_highest_set_bit >= 7) {
        bit_stream_->add_max_8bits((0x7F & number) | 0x80, 8);
        number >>= 7;
        pos_highest_set_bit -= 7;
    }
    bit_stream_->add_max_8bits(0x7F & number, 8);
}

void BaseTypes::injectIntegerNumber(const std::string & value, uint8_t n_bytes, bool is_unsigned) {
    uint32_t number = std::stoi(value);
    if (!is_unsigned) {
        // Todo MBN there might be an error
        if (number < 0) {
            bit_stream_->add_max_8bits(1, 1);
        }
        bit_stream_->add_max_8bits(0, 1);
    }
    number = abs(number);

    _injectUnsignedNumber(number);
}

int32_t BaseTypes::extractIntegerNumber(uint8_t n_bytes, bool is_unsigned) {
    uint32_t output_data = 0;
    int32_t int_number = 0;
    int32_t sign_factor = 1;
    if (!is_unsigned) {
        output_data = bit_stream_->get_max_4bytes(1);
        if (output_data) {
            sign_factor = -1;
        }
    }

    bool read_another_octet = true;
    uint8_t bit_shifter = 0;
    while (read_another_octet) {
        output_data = bit_stream_->get_max_4bytes(8);
        read_another_octet = output_data & 0x0080;
        output_data = output_data & 0x007F;
        int_number = int_number + (output_data << bit_shifter);
        bit_shifter += 7;
    }
    return sign_factor * int_number;
}

void BaseTypes::injectNBitNumber(const std::string & value, uint8_t bits, int32_t offset) {
    int32_t number = std::stoi(value);
    number -= offset;
    bit_stream_->add_max_8bits(number, bits);
}

int32_t BaseTypes::extractNBitNumber(uint8_t bits, int32_t offset) {
    uint32_t output_data = bit_stream_->get_max_4bytes(bits);
    return static_cast<int32_t>(output_data) + offset;
}

void BaseTypes::injectBoolValue(const std::string & value) {
    bool bool_var;
    if (value == "true") {
        bool_var = true;
    } else if (value == "false") {
        bool_var = false;
    } else {
        throw std::runtime_error("The boolean value '" + value + "' can not be parsed!");
    }

    bit_stream_->add_max_8bits(bool_var, 1);
}

bool BaseTypes::extractBoolValue() {
    uint32_t output_data = bit_stream_->get_max_4bytes(1);
    return output_data;
}

void BaseTypes::check_event_code_is_0(std::string current_type_name) {
    uint8_t event_code = bit_stream_->get_max_4bytes(1);

    #ifndef NDEBUG
    std::cout << "check event code for '" << current_type_name << "' --> "
    << std::dec << int(event_code) << std::endl;
    #endif

    if (event_code) {
        throw std::runtime_error("The event code for '" + current_type_name + "' is not 0!");
    }
}

void BaseTypes::add_event_code(std::string current_type_name) {
    #ifndef NDEBUG
    std::cout << "add event code for '" << current_type_name << "' ↴" << std::endl;
    #endif

    bit_stream_->add_max_8bits(0, 1);
}

void BaseTypes::add_event_code_with_n_bits(int8_t event_code, int8_t n_bits, std::string current_type_name) {
#ifndef NDEBUG
    std::cout << "add " << int(n_bits) << "bit(s) event code '" << int(event_code)
        << "' for '" << current_type_name << "' ↴" << std::endl;
#endif

    bit_stream_->add_max_8bits(event_code, n_bits);
}

uint8_t BaseTypes::get_event_code_with_n_bits(int8_t n_bits, std::string current_type_name) {
    uint8_t event_code = bit_stream_->get_max_4bytes(n_bits);

#ifndef NDEBUG
    std::cout << "check multibit event code for '" << current_type_name << "' --> " << std::dec << int(event_code)
    << std::endl;
#endif
    return event_code;
}

void BaseTypes::injectBase64Value(const std::string & value) {
    static const uint8_t base64_values[] = {
            0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
            0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
            0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
            0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
            0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
            0x64, 0x64, 0x64, 0x3E, 0x64, 0x64, 0x64, 0x3F,
            0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B,
            0x3C, 0x3D, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
            0x64, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
            0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
            0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
            0x17, 0x18, 0x19, 0x64, 0x64, 0x64, 0x64, 0x64,
            0x64, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
            0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
            0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30,
            0x31, 0x32, 0x33, 0x64, 0x64, 0x64, 0x64, 0x64};

    // ensure length of string is multiple of 4
    size_t aligned_to_4_length = (value.length() >> 2) << 2;
    if (aligned_to_4_length != value.length()) {
        throw std::runtime_error("The base64 encoded value has not a length with a multiple of 4 ("
            + std::to_string(value.length()) + ")!");
    }

    // find length in bytes and add it to stream
    size_t num_bytes = (value.length() >> 2) * 3;
    size_t num_full_chars = value.length();
    if (value.at(value.length() - 1) == '=') {
        num_bytes--;
        num_full_chars--;
        if (value.at(value.length() - 2) == '=') {
            num_bytes--;
            num_full_chars--;
        }
    }
    _injectUnsignedNumber(num_bytes);
    size_t num_bits = num_bytes << 3;

    size_t i;
    for (i = 0; i < num_full_chars; i++) {
        const auto & current_char = value.at(i);
        if (current_char > 127) [[ unlikely ]] {
            throw std::runtime_error("The character '" + std::string(1, current_char) + "' at position "
                + std::to_string(i) + " can not be encoded to base64!");
        }

        const auto & encoded_value = base64_values[current_char];
        if (encoded_value == 64) [[ unlikely ]] {
            throw std::runtime_error("The character '" + std::string(1, current_char) + "' at position "
                                     + std::to_string(i) + " can not be encoded to base64!");
        }

        if (num_bits < 6) [[ unlikely ]] {  // NOLINT
            bit_stream_->add_max_8bits(encoded_value >> (6 - num_bits), num_bits);
            return;
        } else {
            bit_stream_->add_max_8bits(encoded_value, 6);
            num_bits -= 6;
        }
    }
}


std::string BaseTypes::extractBase64Value() {
    static const char base64_chars[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";

    uint8_t num_bytes = extractIntegerNumber(2, true);
    std::ostringstream out_str;
    uint32_t num_bits = num_bytes * 8;

    #ifndef NDEBUG
        std::cout << "Extract Base64 " << int(num_bytes) << "bytes (" << num_bits << "), "
                  << num_bits / 6 << "charreads & " << num_bits % 6 << "bits compensation" << std::endl;
    #endif

    while (num_bits >= 6) {
        auto v = bit_stream_->get_max_4bytes(6);
        out_str << base64_chars[v];
        num_bits -= 6;
    }

    if (num_bits > 0) {
        auto v = bit_stream_->get_max_4bytes(num_bits);
        out_str << base64_chars[v << (6-num_bits)];
        out_str << '=';
    }

    // Ensure there are a multiple of 4 characters
    uint32_t num_chars = out_str.str().length();
    if (num_chars - ((num_chars >> 2) << 2)) {
        out_str << '=';
    }

    return out_str.str();
}
