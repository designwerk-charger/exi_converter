#include "gtest/gtest.h"

#include "base/input_string_stream.h"


const std::string TEST_INPUT = "{\"V2G_Message\": {\"Header\": {\"SessionID\": \"F05FBD2A935C8EC5\"}, \"Body\": {\"SessionSetupRes\": {\"ResponseCode\": \"OK_NewSessionEstablished\", \"EVSEID\": \"CH123DW123\", \"EVSETimeStamp\": 277130}}}}";  // NOLINT

const char * TEST_LIST = R"({"PaymentOption":["ExternalPayment"]})";


TEST(StringStreamTest_getNextItem, ReturnsFirstTwoItems) {
    InputStringStream ss(TEST_INPUT);

    EXPECT_EQ(ss.get_item_and_move_to_next(), "V2G_Message");
    EXPECT_EQ(ss.get_item_and_move_to_next(), "Header");
}

TEST(StringStreamTest_getNextItem, WorksWithList) {
    InputStringStream ss(TEST_LIST);

    EXPECT_EQ(ss.get_item_and_move_to_next(), "PaymentOption");
    EXPECT_TRUE(ss.is_list_start_move_to_next());
    EXPECT_EQ(ss.get_item_and_move_to_next(), "ExternalPayment");
    EXPECT_TRUE(ss.is_list_end_move_to_next());
}
