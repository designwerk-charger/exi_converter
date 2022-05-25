#include "gtest/gtest.h"

#include "base/bitstream.h"


TEST(BitStreamTest, ThrowInvalidArgumentException_When_InitializingWithLenghZero) {
    EXPECT_THROW(BitStream(NULL, 0), std::invalid_argument);
}

TEST(BitStreamTest_GetNextNBits, ThrowInvalidArgumentException_When_Bits0) {
    BitStream bs;
    uint8_t output_data;
    EXPECT_THROW(bs.get_next_n_bits(0, &output_data), std::invalid_argument);
}

TEST(BitStreamTest_GetNextNBits, ThrowInvalidArgumentException_When_DataIsNull) {
    BitStream bs;
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

TEST(BitStreamTest_GetNextNBits, Returns32Bit) {
    uint8_t five_byte_input[] = {0xDE, 0xAD, 0xBE, 0xAF, 0x23};
    uint32_t output_data = 0;
    BitStream bs(five_byte_input, 5);

    bs.get_next_n_bits(32, reinterpret_cast<uint8_t *>(&output_data));

    ASSERT_EQ(output_data, *reinterpret_cast<uint32_t*>(five_byte_input));
}

TEST(BitStreamTest_GetNextNBits, Returns16bit_when_already1BitRequested) {
    uint8_t five_byte_input[] = {0xDE, 0xAD, 0xBE, 0xAF, 0x23};
    uint8_t rubish = 0;
    uint8_t output_data[2] = {};
    BitStream bs(five_byte_input, 5);

    bs.get_next_n_bits(1, &rubish);
    bs.get_next_n_bits(16, output_data);

    ASSERT_EQ(output_data[0], 0xBD);
    ASSERT_EQ(output_data[1], 0x5B);
}

TEST(BitStreamTest_GetNextNBits, Returns24bit_when_already1ByteRequested) {
    uint8_t five_byte_input[] = {0xDE, 0xAD, 0xBE, 0xAF, 0x23};
    uint8_t rubish = 0;
    uint8_t output_data[3] = {};
    BitStream bs(five_byte_input, 5);

    bs.get_next_n_bits(8, &rubish);
    bs.get_next_n_bits(24, output_data);

    ASSERT_EQ(output_data[0], 0xAD);
    ASSERT_EQ(output_data[1], 0xBE);
    ASSERT_EQ(output_data[2], 0xAF);
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

TEST(BitStreamTest_GetNextNBits, Returns2singleBitsAnd7bits_when_2SingleBitsAnd7BitsRequested) {
    uint8_t four_byte_input[] = {0b01000101, 0b01111010, 0b10001000, 0x7F};
    uint32_t output_data1 = 0;
    uint32_t output_data2 = 0;
    uint32_t output_data3 = 0;

    BitStream bs(four_byte_input, 5);

    bs.get_next_n_bits(1, reinterpret_cast<uint8_t *>(&output_data1));
    bs.get_next_n_bits(1, reinterpret_cast<uint8_t *>(&output_data2));
    bs.get_next_n_bits(7, reinterpret_cast<uint8_t *>(&output_data3));

    ASSERT_EQ(output_data1, 0x00);
    ASSERT_EQ(output_data2, 0x01);
    ASSERT_EQ(output_data3, 0b00001010);
}

TEST(BitStreamTest_GetNextNBits, Returns8Byte) {
    uint8_t byte_input[] = {0xF0, 0x5F, 0xBD, 0x2A, 0x93, 0x5C, 0x8E, 0xC5, 0x17};
    BitStream bs(byte_input, 9);
    uint8_t databytes[8];

    bs.get_next_n_bits(8*8, databytes);

    for (int i=0; i < 8; i++) {
        ASSERT_EQ(databytes[i], byte_input[i]);
    }
}


TEST(BitStreamTest_GetMax4Bytes, ReturnsOneBit) {
    uint8_t five_byte_input[] = {0xDE, 0xAD, 0xBE, 0xAF, 0x23};
    BitStream bs(five_byte_input, 5);

    uint32_t output_data = bs.get_max_4bytes(1);

    ASSERT_EQ(output_data, 0x01);
}

TEST(BitStreamTest_GetMax4Bytes, ReturnsOneByte) {
    uint8_t five_byte_input[] = {0xDE, 0xAD, 0xBE, 0xAF, 0x23};
    BitStream bs(five_byte_input, 5);

    uint32_t output_data = bs.get_max_4bytes(8);

    ASSERT_EQ(output_data, 0xDE);
}

TEST(BitStreamTest_GetMax4Bytes, ReturnsOneByte_when_alreadyOneBitRequested) {
    uint8_t five_byte_input[] = {0xDE, 0xAD, 0xBE, 0xAF, 0x23};
    BitStream bs(five_byte_input, 5);

    bs.get_max_4bytes(1);
    uint32_t output_data = bs.get_max_4bytes(8);

    ASSERT_EQ(output_data, 0xBD);
}

TEST(BitStreamTest_GetMax4Bytes, ReturnsOneByte_when_alreadyTwoBitRequested) {
    uint8_t five_byte_input[] = {0xDE, 0xAD, 0xBE, 0xAF, 0x23};
    BitStream bs(five_byte_input, 5);

    bs.get_max_4bytes(1);
    bs.get_max_4bytes(1);
    uint32_t output_data = bs.get_max_4bytes(8);

    ASSERT_EQ(output_data, 0x7A);
}

TEST(BitStreamTest_GetMax4Bytes, ReturnsOneByte_when_already3ByteRequested) {
    uint8_t five_byte_input[] = {0xDE, 0xAD, 0xBE, 0xAF, 0x23, 0xA2, 0xE1, 0x57, 0x17};
    BitStream bs(five_byte_input, 9);

    bs.get_max_4bytes(24);
    uint32_t output_data = bs.get_max_4bytes(8);

    ASSERT_EQ(output_data, 0xAF);
}

TEST(BitStreamTest_GetMax4Bytes, Returns4Byte) {
    uint8_t five_byte_input[] = {0xDE, 0xAD, 0xBE, 0xAF, 0x23, 0xA2, 0xE1, 0x57, 0x17};
    BitStream bs(five_byte_input, 9);

    uint32_t output_data = bs.get_max_4bytes(32);

    ASSERT_EQ(output_data, 0xDEADBEAF);
}

TEST(BitStreamTest_GetMax4Bytes, Returns4Byte_when_alreadyOneByteRequested) {
    uint8_t five_byte_input[] = {0xDE, 0xAD, 0xBE, 0xAF, 0x23, 0xA2, 0xE1, 0x57, 0x17};
    BitStream bs(five_byte_input, 9);

    bs.get_max_4bytes(8);
    uint32_t output_data = bs.get_max_4bytes(32);

    ASSERT_EQ(output_data, 0xADBEAF23);
}

TEST(BitStreamTest_GetMax4Bytes, Returns4Byte_when_alreadyOneBitRequested) {
    uint8_t five_byte_input[] = {0xDE, 0xAD, 0xBE, 0xAF, 0x23, 0xA2, 0xE1, 0x57, 0x17};
    BitStream bs(five_byte_input, 9);

    bs.get_max_4bytes(1);
    uint32_t output_data = bs.get_max_4bytes(32);

    ASSERT_EQ(output_data, 0xBD5B7D5E);
}


TEST(BitStreamTest_GetMax4Bytes, Returns2Byte_when_alreadyTwoBitRequested) {
    uint8_t five_byte_input[] = {0xDE, 0x72, 0xBE, 0xAF, 0x23, 0xA2, 0xE1, 0x57, 0x17};
    BitStream bs(five_byte_input, 9);

    bs.get_max_4bytes(1);
    bs.get_max_4bytes(1);
    uint32_t output_data = bs.get_max_4bytes(16);

    ASSERT_EQ(output_data, 0x79CA);
}

TEST(BitStreamTest_AddBits, ThrowInvalidArgumentException_When_Bits0) {
    BitStream bs;
    EXPECT_THROW(bs.add_max_8bits(0xAB, 0), std::invalid_argument);
}

TEST(BitStreamTest_AddBits, ThrowInvalidArgumentException_When_DataIsNull) {
    BitStream bs;
    EXPECT_THROW(bs.add_max_8bits(0xAB, 9), std::range_error);
}

TEST(bitStreamTest_Addbits, AddsFirstBit_When_OneBitAdded) {
    uint8_t input_data = 0x01;
    std::vector<uint8_t>exi_data;
    BitStream bs;

    bs.add_max_8bits(input_data, 1);
    exi_data = bs.get_exi_data();

    ASSERT_EQ(exi_data[0], 0x80);
}

TEST(bitStreamTest_Addbits, AddsFirst2Bits_When_TwoBitAdded) {
    uint8_t input_data = 0x02;
    std::vector<uint8_t>exi_data;
    BitStream bs;

    bs.add_max_8bits(input_data, 2);
    exi_data = bs.get_exi_data();

    ASSERT_EQ(exi_data[0], 0x80);
}

TEST(bitStreamTest_Addbits, AddsTwoConsequtive2Bits_When_TwoBitsAddedWithTwoFunctionCalls) {
    uint8_t input_data_1 = 0x01;
    uint8_t input_data_2 = 0x01;
    std::vector<uint8_t>exi_data;
    BitStream bs;

    bs.add_max_8bits(input_data_1, 1);
    bs.add_max_8bits(input_data_2, 1);
    exi_data = bs.get_exi_data();

    ASSERT_EQ(exi_data[0], 0xC0);
}

TEST(bitStreamTest_Addbits, Adds1Byte_When_OneByteAdded) {
    uint8_t input_data = 0xDE;
    std::vector<uint8_t>exi_data;
    std::vector<uint8_t>test_vector;
    test_vector.push_back(input_data);
    BitStream bs;

    bs.add_max_8bits(input_data, 8);
    exi_data = bs.get_exi_data();

    ASSERT_EQ(exi_data, test_vector);
}

TEST(bitStreamTest_Addbits, Adds1BitAnd1Byte_When_1BitAnd1ByteAddedIn2FunctionCalls) {
    uint8_t input_data_1 = 0x01;
    uint8_t input_data_2 = 0x01;
    std::vector<uint8_t>test_vector;
    std::vector<uint8_t>exi_data;
    BitStream bs;
    test_vector.push_back(0x80);
    test_vector.push_back(0x80);

    bs.add_max_8bits(input_data_1, 1);
    bs.add_max_8bits(input_data_2, 8);
    exi_data = bs.get_exi_data();

    ASSERT_EQ(exi_data, test_vector);
}

TEST(bitStreamTest_Addbits, Adds2Bytes_When_2BytesAdded) {
    uint8_t input_data[] = {0xDE, 0xAD};
    std::vector<uint8_t>test_vector;
    test_vector.push_back(input_data[0]);
    test_vector.push_back(input_data[1]);
    std::vector<uint8_t>exi_data;
    BitStream bs;

    bs.add_bytes(input_data, 2);
    exi_data = bs.get_exi_data();

    ASSERT_EQ(exi_data, test_vector);
}

TEST(bitStreamTest_Addbits, Adds2ByteAnd1Bit_When_2ByteAnd1BitAdded) {
    uint8_t input_data[] = {0xAD, 0xEE, 0x01};
    std::vector<uint8_t>test_vector;
    test_vector.push_back(input_data[0]);
    test_vector.push_back(input_data[1]);
    test_vector.push_back(0x80);
    std::vector<uint8_t>exi_data;
    BitStream bs;

    bs.add_bytes(input_data, 2);
    bs.add_max_8bits(input_data[2], 1);
    exi_data = bs.get_exi_data();

    ASSERT_EQ(exi_data, test_vector);
}
