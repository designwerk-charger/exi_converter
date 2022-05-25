#include "gtest/gtest.h"

#include "base/stringstream.h"


const std::string TEST_INPUT = "{\"V2G_Message\": {\"Header\": {\"SessionID\": \"F05FBD2A935C8EC5\"}, \"Body\": {\"SessionSetupRes\": {\"ResponseCode\": \"OK_NewSessionEstablished\", \"EVSEID\": \"CH123DW123\", \"EVSETimeStamp\": 277130}}}}";  // NOLINT

TEST(StringStreamTest, CanBeCreated_when_InputDataIsEmpty) {
    EXPECT_NO_THROW(StringStream(""));
}

TEST(StringStreamTest_getNextItem, ReturnsFirstTwoItems) {
    StringStream ss(TEST_INPUT);

    EXPECT_EQ(ss.get_next_item(), "V2G_Message");
    EXPECT_EQ(ss.get_next_item(), "Header");
}

TEST(StringStreamTest_createStream, CreateKeyAndValue) {
    StringStream ss("");

    ss.start_key("key");
    ss.add_value("value");
    ss.end_key();

    EXPECT_EQ(ss.get_full_stream(), "{\"key\":\"value\"}");
}

TEST(StringStreamTest_createStream, CreateKeyAndKeyWithValue) {
    StringStream ss("");

    ss.start_key("key1");
    ss.start_key("key2");
    ss.add_value("value");
    ss.end_key();
    ss.end_key();

    EXPECT_EQ(ss.get_full_stream(), "{\"key1\":{\"key2\":\"value\"}}");
}


TEST(StringStreamTest_createStream, CreateKeyWithTwoValues) {
    StringStream ss("");

    ss.start_key("key1");
    ss.add_value("value1");
    ss.end_key();
    ss.start_key("key2");
    ss.add_value("value2");
    ss.end_key();

    EXPECT_EQ(ss.get_full_stream(), "{\"key1\":\"value1\",\"key2\":\"value2\"}");
}

TEST(StringStreamTest_createStream, CreatingFullMessageString) {
    StringStream ss("");

    ss.start_key("V2G_Message");
    ss.start_key("Header");
    ss.start_key("SessionID");
    ss.add_value("F05FBD2A935C8EC5");
    ss.end_key();
    ss.end_key();
    ss.start_key("Body");
    ss.start_key("SessionSetupRes");

    ss.start_key("ResponseCode");
    ss.add_value("OK_NewSessionEstablished");
    ss.end_key();
    ss.start_key("EVSEID");
    ss.add_value("CH123DW123");
    ss.end_key();
    ss.start_key("EVSETimeStamp");
    ss.add_value(277130);
    ss.end_key();

    ss.end_key();
    ss.end_key();
    ss.end_key();

    EXPECT_EQ(ss.get_full_stream(), "{\"V2G_Message\":{\"Header\":{\"SessionID\":\"F05FBD2A935C8EC5\"},\"Body\":{\"SessionSetupRes\":{\"ResponseCode\":\"OK_NewSessionEstablished\",\"EVSEID\":\"CH123DW123\",\"EVSETimeStamp\":277130}}}}");  // NOLINT
}

TEST(StringStreamTest_createStream, AddsEmptyValue_when_KeyStartedWithoutValue) {
    StringStream ss("");

    ss.start_key("key1");
    ss.end_key();

    EXPECT_EQ(ss.get_full_stream(), R"({"key1":{}})");
}
