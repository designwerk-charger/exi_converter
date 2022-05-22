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
