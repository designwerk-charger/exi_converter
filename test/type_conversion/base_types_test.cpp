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

TEST(BaseTypeTest, GetUri) {
    uint8_t str_data[]  = { ')', 'h', 't', 't', 'p', ':', '/', '/', 'w', 'w', 'w', '.', 'w',
                           '3', '.', 'o', 'r', 'g', '/', '2', '0', '0', '1', '/', '0', '4', '/',
                           'x', 'm', 'l', 'e', 'n', 'c', '#', 's', 'h', 'a', '2', '5', '6', 'B',
                           '\\', 0x01};
    BitStream bs(str_data, 43);
    BaseTypes bt(&bs);
    std::string bt_out;

    bt_out = bt.extractUri();

    ASSERT_EQ(bt_out, "http://www.w3.org/2001/04/xmlenc#sha256");
}

TEST(BaseTypeTest, GetStringFromExiStream_When_AskedForString) {
    uint8_t str_data[]  = {0x07, 'H', 'E', 'L', 'L', 'O'};
    BitStream bs(str_data, 6);
    BaseTypes bt(&bs);
    std::string bt_out;

    bt_out = bt.extractString();

    ASSERT_EQ(bt_out, "HELLO");
}

TEST(BaseTypeTest, GetStringFromExiStreamThrowsRuntimeException_When_StringLengthSmaller2) {
    uint8_t str_data[]  = {0x01, 'H', 'E', 'L', 'L', 'O'};
    BitStream bs(str_data, 6);
    BaseTypes bt(&bs);
    std::string bt_out;

    ASSERT_THROW(bt.extractString(), std::runtime_error);
}

TEST(BaseTypeTest, GetHexStringFromExiStream_When_AskedForHexString) {
    uint8_t exi_data[] = {0x04, 0xDE, 0xAD, 0xBE, 0xAF};
    BitStream bs(exi_data, 5);
    BaseTypes bt(&bs);
    std::string bt_out;

    bt_out = bt.extractHexBin(4);

    ASSERT_EQ(bt_out, "DEADBEAF");
}

TEST(BaseTypeTest, BoolWasAddedToExi_When_BoolWasInjected) {
    BitStream bs;
    BaseTypes bt(&bs);
    std::vector<uint8_t>exi_data;
    std::vector<uint8_t>test_vector;
    test_vector.push_back(0x80);

    bt.injectBoolValue(true);
    exi_data = bs.get_exi_data();

    ASSERT_EQ(exi_data, test_vector);
}

TEST(BaseTypeTest, UintWasAddedToExi_When_UintWasInjected) {
    BitStream bs;
    BaseTypes bt(&bs);
    std::vector<uint8_t>exi_data;
    std::vector<uint8_t>test_vector;
    test_vector.push_back(0b10001010);
    test_vector.push_back(0b11110101);
    test_vector.push_back(0b00010000);

    bt.injectIntegerNumber(277130, true);
    exi_data = bs.get_exi_data();

    ASSERT_EQ(exi_data, test_vector);
}

TEST(BaseTypeTest, IntWasAddedToExi_When_IntWasInjected) {
    BitStream bs;
    BaseTypes bt(&bs);
    std::vector<uint8_t>exi_data;
    std::vector<uint8_t>test_vector;
    test_vector.push_back(0b01000101);
    test_vector.push_back(0b01111010);
    test_vector.push_back(0b10001000);
    test_vector.push_back(0b0);

    bt.injectIntegerNumber(277130, false);
    exi_data = bs.get_exi_data();

    ASSERT_EQ(exi_data, test_vector);
}

TEST(BaseTypeTest, StringWasAddedToExi_When_StringWasInjected) {
    BitStream bs;
    BaseTypes bt(&bs);
    std::vector<uint8_t>exi_data;
    std::vector<uint8_t>test_vector;
    test_vector.push_back(0b00001100);
    test_vector.push_back(0b01000011);
    test_vector.push_back(0b01001000);
    test_vector.push_back(0b00110001);
    test_vector.push_back(0b00110010);
    test_vector.push_back(0b00110011);
    test_vector.push_back(0b01000100);
    test_vector.push_back(0b01010111);
    test_vector.push_back(0b00110001);
    test_vector.push_back(0b00110010);
    test_vector.push_back(0b00110011);

    bt.injectString("CH123DW123");
    exi_data = bs.get_exi_data();

    ASSERT_EQ(exi_data, test_vector);
}

TEST(BaseTypeTest, BinDataWasAddedToExi_When_HexStringWasInjected) {
    BitStream bs;
    BaseTypes bt(&bs);
    std::vector<uint8_t>exi_data;
    std::vector<uint8_t>test_vector;
    test_vector.push_back(0b00001000);
    test_vector.push_back(0b11110000);
    test_vector.push_back(0b01011111);
    test_vector.push_back(0b10111101);
    test_vector.push_back(0b00101010);
    test_vector.push_back(0b10010011);
    test_vector.push_back(0b01011100);
    test_vector.push_back(0b10001110);
    test_vector.push_back(0b11000101);

    bt.injectHexBin("F05FBD2A935C8EC5");
    exi_data = bs.get_exi_data();

    ASSERT_EQ(exi_data, test_vector);
}

TEST(BaseTypeTest, ExtractBase64_fromChargeParameterDiscoveryRes) {
    std::vector<uint8_t> raw_data({0x20, 0x14, 0xe3, 0xd8, 0x7a, 0xe2, 0x42, 0x4b, 0x6c, 0x30, 0x5f, 0xd5, 0xf1,
                                    0x8a, 0x24, 0xFE, 0x2F, 0x2F, 0x68, 0x7C, 0xd6, 0x58, 0xd5, 0x9b, 0xbe, 0x51,
                                    0xa7, 0xCE, 0x3F, 0x5C, 0xF0, 0x86, 0xF0, 0x00});
    BitStream bs(raw_data);
    BaseTypes bt(&bs);
    std::string bt_out;

    bt_out = bt.extractBase64Value();

    ASSERT_EQ(bt_out, "FOPYeuJCS2wwX9XxiiT+Ly9ofNZY1Zu+UafOP1zwhvA=");
}

TEST(BaseTypeTest, ExtractBase64_TestData1) {
    std::vector<uint8_t> raw_data({0x01, 'f'});
    BitStream bs(raw_data);
    BaseTypes bt(&bs);
    std::string bt_out;

    bt_out = bt.extractBase64Value();

    ASSERT_EQ(bt_out, "Zg==");
}

TEST(BaseTypeTest, ExtractBase64_TestData2) {
    std::vector<uint8_t> raw_data({0x02, 'f', 'o'});
    BitStream bs(raw_data);
    BaseTypes bt(&bs);
    std::string bt_out;

    bt_out = bt.extractBase64Value();

    ASSERT_EQ(bt_out, "Zm8=");
}

TEST(BaseTypeTest, ExtractBase64_TestData3) {
    std::vector<uint8_t> raw_data({0x03, 'f', 'o', 'o'});
    BitStream bs(raw_data);
    BaseTypes bt(&bs);
    std::string bt_out;

    bt_out = bt.extractBase64Value();

    ASSERT_EQ(bt_out, "Zm9v");
}
