#include <iostream>
#include "base_types.h"

BaseTypes::BaseTypes(BitStream * bit_stream) : bit_stream_(bit_stream) {}


void BaseTypes::injectHexBin(std::string) { }

std::string BaseTypes::extractHexBin(uint8_t max_length) {
    return "";
}

uint32_t BaseTypes::extractNBitsForEnum(uint32_t n_bits) {
    uint32_t output_data = 0;
    bit_stream_->get_next_n_bits(n_bits, reinterpret_cast<uint8_t *>(&output_data));
    return output_data;
}

void BaseTypes::injectString(std::string value) { }

std::string BaseTypes::extractString() {
    uint8_t length_str;
    bit_stream_->get_next_n_bits(8, &length_str);
    uint8_t* out_chars = new uint8_t[length_str + 1];

    bit_stream_->get_next_n_bits((length_str) * 8, out_chars);
    out_chars[length_str] = 0;

    std::string out_string(reinterpret_cast<const char *>(out_chars));
    delete [] out_chars;
    return out_string;
}

void BaseTypes::injectIntegerNumber(int32_t number, bool is_unsigned) { }

int32_t BaseTypes::extractIntegerNumber(uint8_t n_bytes, bool is_unsigned) {
    uint32_t output_data = 0;
    int32_t int_number = 0;
    bool is_negative = false;
    if (!is_unsigned) {
        bit_stream_->get_next_n_bits(1, reinterpret_cast<uint8_t *>(&output_data));
        is_negative = output_data;
    }

    bool read_another_octet = true;
    uint8_t bit_shifter = 0;
    while (read_another_octet) {
        bit_stream_->get_next_n_bits(1, reinterpret_cast<uint8_t *>(&output_data));
        read_another_octet = output_data;
        bit_stream_->get_next_n_bits(7, reinterpret_cast<uint8_t *>(&output_data));
        int_number = int_number + (output_data << bit_shifter);
        bit_shifter += 7;
    }
    if (is_negative) {
        return (-1 * int_number);
    }
    return int_number;
}

void BaseTypes::injectBoolValue(bool) { }

bool BaseTypes::extractBoolValue() {
    uint32_t output_data = 0;
    bit_stream_->get_next_n_bits(1, reinterpret_cast<uint8_t *>(&output_data));
    return output_data;
}

void BaseTypes::injectBase64Value(std::string value) { }

// todo llr: string??
std::string BaseTypes::extractBase64Value() {
    return "";
}

