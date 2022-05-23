#include "base_types.h"

BaseTypes::BaseTypes(BitStream * bit_stream) : bit_stream_(bit_stream) {}


void BaseTypes::injectHexBin(std::string) { }

std::string BaseTypes::extractHexBin(uint8_t max_length) {
    return "";
}

uint32_t BaseTypes::extractNBitsForEnum(uint32_t n_bits) {
}

void BaseTypes::injectString(std::string value) { }

std::string BaseTypes::extractString() {
    return "";
}

void BaseTypes::injectNaturalNumber(int32_t number) { }

int32_t BaseTypes::extractIntegerNumber(uint8_t n_bytes, bool is_unsigned) {
    return 0;
}

void BaseTypes::injectBoolValue(bool) { }

bool BaseTypes::extractBoolValue() {
    return false;
}

void BaseTypes::injectBase64Value(std::string value) { }

std::string BaseTypes::extractBase64Value() {
    return "";
}

