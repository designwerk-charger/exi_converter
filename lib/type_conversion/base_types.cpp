#include <iostream>
#include <sstream>
#include <iomanip>
#include "base_types.h"

BaseTypes::BaseTypes(BitStream * bit_stream) : bit_stream_(bit_stream) {}


void BaseTypes::injectHexBin(std::string hex_string) {
    uint8_t length_data_array;
    length_data_array = hex_string.length() >> 1;
    bit_stream_->add_max_8bits(length_data_array, 8);

    uint8_t* data = new uint8_t[length_data_array];

    for (uint8_t byte=0; byte < length_data_array; byte++) {
        data[byte] = std::stoul(hex_string.substr(byte * 2, 2), nullptr, 16);
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
    uint32_t output_data = 0;
    bit_stream_->get_next_n_bits(n_bits, reinterpret_cast<uint8_t *>(&output_data));
    return output_data;
}

void BaseTypes::injectNBitsForEnum(uint8_t value, uint8_t n_bits) {
    bit_stream_->add_max_8bits(value, n_bits);
}

void BaseTypes::injectString(std::string value) {
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

void BaseTypes::injectUri(std::string value) {
    injectString(value);
}

std::string BaseTypes::extractUri() {
    return extractString();
}

void BaseTypes::injectIntegerNumber(int32_t number, bool is_unsigned) {
    if (!is_unsigned) {
        if (number < 0) {
            injectBoolValue(true);
        }
        injectBoolValue(false);
    }
    number = abs(number);

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

int32_t BaseTypes::extractIntegerNumber(uint8_t n_bytes, bool is_unsigned) {
    uint32_t output_data = 0;
    int32_t int_number = 0;
    int32_t sign_factor = 1;
    if (!is_unsigned) {
        bit_stream_->get_next_n_bits(1, reinterpret_cast<uint8_t *>(&output_data));
        if (output_data) {
            sign_factor = -1;
        }
    }

    bool read_another_octet = true;
    uint8_t bit_shifter = 0;
    while (read_another_octet) {
        bit_stream_->get_next_n_bits(8, reinterpret_cast<uint8_t *>(&output_data));
        read_another_octet = output_data & 0x0080;
        output_data = output_data & 0x007F;
        int_number = int_number + (output_data << bit_shifter);
        bit_shifter += 7;
    }
    return sign_factor * int_number;
}

void BaseTypes::injectNBitNumber(int32_t number, uint8_t bits, int32_t offset) { }

int32_t BaseTypes::extractNBitNumber(uint8_t bits, int32_t offset) {
    uint32_t output_data = bit_stream_->get_max_4bytes(bits);
    return static_cast<int32_t>(output_data) + offset;
}

void BaseTypes::injectBoolValue(bool bool_var) {
    bit_stream_->add_max_8bits(bool_var, 1);
}

bool BaseTypes::extractBoolValue() {
    uint32_t output_data = 0;
    bit_stream_->get_next_n_bits(1, reinterpret_cast<uint8_t *>(&output_data));
    return output_data;
}

void BaseTypes::check_event_code_is_0(std::string current_type_name) {
    uint8_t event_code;
    bit_stream_->get_next_n_bits(1, &event_code);

    #ifndef NDEBUG
    std::cout << "check event code for '" << current_type_name << "' --> "
    << std::dec << int(event_code) << std::endl;
    #endif

    if (event_code) {
        throw std::runtime_error("The event code for '" + current_type_name + "' is not 0!");
    }
}

void BaseTypes::add_event_code(std::string current_type_name) {
    bit_stream_->add_max_8bits(0, 1);

    #ifndef NDEBUG
    std::cout << "add event code for '" << current_type_name << "'";
    #endif
}

uint8_t BaseTypes::get_event_code_with_n_bits(int8_t n_bits, std::string current_type_name) {
    uint8_t event_code;
    bit_stream_->get_next_n_bits(n_bits, &event_code);

#ifndef NDEBUG
    std::cout << "check multibit event code for '" << current_type_name << "' --> " << std::dec << int(event_code)
    << std::endl;
#endif
    return event_code;
}

void BaseTypes::injectBase64Value(std::string value) { }


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
