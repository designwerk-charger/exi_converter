#pragma once

#include "type_conversion/base_types.h"
#include "gmock/gmock.h"

class MockBaseTypes : public BaseTypes {
public:
    MockBaseTypes(BitStream * bs) : BaseTypes(bs) {}  // NOLINT
    // uint32_t BaseTypes::extractNBitsForEnum(uint32_t n_bits)
    MOCK_METHOD(uint32_t , extractNBitsForEnum, (uint32_t), (override));

    // void injectNBitsForEnum(uint8_t value, uint8_t n_bits);
    MOCK_METHOD(void , injectNBitsForEnum, (uint8_t, uint8_t), (override));
};
