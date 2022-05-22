#include "gtest/gtest.h"

#include "base/bitstream.h"

/*
TEST(BitStreamTest, TestGetCorrectBitsOfstream) {
    //uint8_t raw_data [3] = {0b01100000, 0b01011010, 0b00001111};
    uint32_t raw_data [3] = {0xAFFEDEAD, 0xBEAF0000, 0xABCDEFFF};
    BitStream bs(raw_data, 8);

    // uint32_t bit_seq =
    uint8_t data[3];
    bs.get_next_n_bits(2, data);
    EXPECT_EQ(2, *data);
    bs.get_next_n_bits(5, data);
    EXPECT_EQ(23, *data);
    bs.get_next_n_bits(14, data);
    EXPECT_EQ(0b11111111011011, *data);
} */

TEST(BitStreamTest, ThrowInvalidArgumentException_When_InitializingWithLenghZero) {
    EXPECT_THROW(BitStream(NULL, 0), std::invalid_argument);
}

TEST(BitStreamTest_GetNextNBits, ThrowInvalidArgumentException_When_Bits0) {
    BitStream bs(NULL, 1);
    uint8_t output_data;
    EXPECT_THROW(bs.get_next_n_bits(0, &output_data), std::invalid_argument);
}

TEST(BitStreamTest_GetNextNBits, ThrowInvalidArgumentException_When_DataIsNull) {
    BitStream bs(NULL, 1);
    EXPECT_THROW(bs.get_next_n_bits(2, NULL), std::invalid_argument);
}

TEST(BitStreamTest_GetNextNBits, ReturnsFirstBit_When_OneBitRequested) {
    uint8_t msb_set = 0x80;
    uint8_t output_data = 0xFF;
    BitStream bs(&msb_set, 1);

    bs.get_next_n_bits(1, &output_data);

    ASSERT_EQ(output_data, 0x01);
}

TEST(BitStreamTest_GetNextNBits, ReturnsFirstTwoBit_When_TwoBitRequested) {
    uint8_t msb_set = 0x80;
    uint8_t output_data = 0xFF;
    BitStream bs(&msb_set, 1);

    bs.get_next_n_bits(2, &output_data);

    ASSERT_EQ(output_data, 0x02);
}

TEST(BitStreamTest_GetNextNBits, ReturnsWholeByte_When_8BitRequested) {
    uint8_t one_byte_input = 0x83;
    uint8_t output_data = 0xFF;
    BitStream bs(&one_byte_input, 1);

    bs.get_next_n_bits(8, &output_data);

    ASSERT_EQ(output_data, one_byte_input);
}

TEST(BitStreamTest_GetNextNBits, ThrowsRangeError_When_RequestedNumberOfBitsExceedsAvailableBits) {
    uint8_t one_byte_input = 0x83;
    uint8_t output_data = 0xFF;
    BitStream bs(&one_byte_input, 1);

    ASSERT_THROW(bs.get_next_n_bits(9, &output_data), std::range_error);
}

TEST(BitStreamTest_GetNextNBits, ReturnsTwoConsequtiveBits_When_2BitsRequestedWithTwoFunctionCalls) {
    uint8_t one_byte_input = 0xAA;
    uint8_t output_data1 = 0xFF;
    uint8_t output_data2 = 0xFF;
    BitStream bs(&one_byte_input, 1);

    bs.get_next_n_bits(1, &output_data1);
    bs.get_next_n_bits(1, &output_data2);

    ASSERT_EQ(output_data1, 0x01);
    ASSERT_EQ(output_data2, 0x00);
}

TEST(BitStreamTest_GetNextNBits, Returns32bit_when_32bitRequested) {
    uint8_t five_byte_input[] = {0xDE, 0xAD, 0xBE, 0xAF, 0x23};
    uint32_t output_data = 0;
    BitStream bs(five_byte_input, 5);

    bs.get_next_n_bits(32, reinterpret_cast<uint8_t *>(&output_data));

    ASSERT_EQ(output_data, *reinterpret_cast<uint32_t*>(five_byte_input));
}

TEST(BitStreamTest_GetNextNBits, ReturnsBitAndByte_when_OneBitAndOneByteRequested) {
    uint8_t five_byte_input[] = {0xDE, 0xAD, 0xBE, 0xAF, 0x23};
    uint32_t output_data1 = 0;
    uint32_t output_data2 = 0;

    BitStream bs(five_byte_input, 5);

    bs.get_next_n_bits(1, reinterpret_cast<uint8_t *>(&output_data1));
    bs.get_next_n_bits(8, reinterpret_cast<uint8_t *>(&output_data2));

    ASSERT_EQ(output_data1, 0x01);
    // 0xDE -> 11011110
    // 0xAD -> 10101101
    // --> 10111101 -> 0xBD
    ASSERT_EQ(output_data2, 0xBD);  // 0xBD
}
