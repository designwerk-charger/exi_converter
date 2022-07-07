#pragma once

#include "base/bitstream.h"
#include "gmock/gmock.h"

class MockBitStream : public BitStream {
public:
    MockBitStream() : BitStream() {}  // NOLINT

    // void add_max_8bits(uint8_t data, uint8_t num_bits);
    MOCK_METHOD(void , add_max_8bits, (uint8_t, uint8_t), (override));
};
