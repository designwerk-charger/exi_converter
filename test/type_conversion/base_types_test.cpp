#include "base_types.h"

#include "gtest/gtest.h"


TEST(BaseTypeTest, GetBoolFromExiStream_When_AskedForBool) {
    uint8_t msb_set = 0x80;
    BitStream bs(&msb_set, 1);
    BaseTypes bt(&bs);
    bool bt_out;

    bt_out = bt.extractBoolValue();

    ASSERT_EQ(true, bt_out);
}

TEST(BaseTypeTest, Get1ByteIntegerFromExiStream_When_AskedForUnsignedInteger) {
    uint8_t exi_data[] = {0x75, 0xCC};
    BitStream bs(exi_data, 2);
    BaseTypes bt(&bs);
    int32_t int_output;

    int_output = bt.extractIntegerNumber(1, true);

    ASSERT_EQ(0x75, int_output);
}

TEST(BaseTypeTest, Get3ByteIntegerFromExiStream_When_AskedForUnsignedInteger) {
    uint8_t exi_data[] = {0b10001010, 0b11110101, 0b00010000};  // Example SessionSetup: timestamp 277130
    BitStream bs(exi_data, 3);
    BaseTypes bt(&bs);
    int32_t int_output;

    int_output = bt.extractIntegerNumber(1, true);

    ASSERT_EQ(277130, int_output);
}

TEST(BaseTypeTest, GetIntegerFromExiStream_When_AskedForNegativeSignedInteger) {
    uint8_t exi_data[] = {0b01000101, 0b01111010, 0b10001000, 0x7F};  // added minus sign to timestamp from sessionstop
    BitStream bs(exi_data, 4);
    BaseTypes bt(&bs);
    int32_t int_output;

    int_output = bt.extractIntegerNumber(1, false);

    ASSERT_EQ(277130, int_output);
}

TEST(BaseTypeTest, GetStringFromExiStream_When_AskedForString) {
    uint8_t str_data[]  = {0x05, 'H', 'E', 'L', 'L', 'O'};
    BitStream bs(str_data, 6);
    BaseTypes bt(&bs);
    std::string bt_out;

    bt_out = bt.extractString();

    ASSERT_EQ(bt_out, "HELLO");
}
