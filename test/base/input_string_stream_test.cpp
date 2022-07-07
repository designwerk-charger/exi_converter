#include "gtest/gtest.h"

#include "base/input_string_stream.h"


const char * TEST_INPUT = R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"SessionSetupRes":{"ResponseCode":"OK_NewSessionEstablished","EVSEID":"UK123E1234","EVSETimeStamp":1652790080}}}})";
const char * TEST_LIST = R"({"PaymentOption":["ExternalPayment"]})";



TEST(StringStreamTest_ItemSplitting, ThrowsInvalidArgument_when_InputNotStartingWithCurledBrakets) {
    EXPECT_THROW(InputStringStream::split_items("no_brakets"), std::invalid_argument);
}

TEST(StringStreamTest_ItemSplitting, ContainsSingleKeyAndStringValue) {
    auto items = InputStringStream::split_items(R"({"Key":"Value"})");
    EXPECT_EQ(items.at(0).first, "Key");
    EXPECT_EQ(items.at(1).first, "Value");
}

TEST(StringStreamTest_ItemSplitting, ContainsSingleKeyAndStringValueWithEscapeCharacters) {
    auto items = InputStringStream::split_items(R"({"V2G_M\"essage":"Value"})");
    EXPECT_EQ(items.at(0).first, R"(V2G_M\"essage)");
    EXPECT_EQ(items.at(1).first, "Value");
}

TEST(StringStreamTest_ItemSplitting, ContainsSingleKeyAndIntValue) {
    auto items = InputStringStream::split_items(R"({"Key":12})");
    EXPECT_EQ(items.at(0).first, "Key");
    EXPECT_EQ(items.at(1).first, "12");
}

TEST(StringStreamTest_ItemSplitting, ContainsTwoKeysAndStringValue) {
    auto items = InputStringStream::split_items(R"({"Key1":{"Key2":"Value"}})");
    EXPECT_EQ(items.at(0).first, "Key1");
    EXPECT_EQ(items.at(1).first, "Key2");
    EXPECT_EQ(items.at(2).first, "Value");
}

TEST(StringStreamTest_ItemSplitting, ContainsKeyWithTwoArrayStrings) {
    auto items = InputStringStream::split_items(R"({"Key":["Arr1","Arr2"]})");
    EXPECT_EQ(items.at(0).first, "Key");
    EXPECT_EQ(items.at(1).first, "[");
    EXPECT_EQ(items.at(2).first, "Arr1");
    EXPECT_EQ(items.at(3).first, "Arr2");
    EXPECT_EQ(items.at(4).first, "]");
}

TEST(StringStreamTest_ItemSplitting, ContainsKeyWithOneArrayKeyValue) {
    auto items = InputStringStream::split_items(R"({"Key":[{"Subkey":"Value"}]})");
    EXPECT_EQ(items.at(0).first, "Key");
    EXPECT_EQ(items.at(1).first, "[");
    EXPECT_EQ(items.at(2).first, "Subkey");
    EXPECT_EQ(items.at(3).first, "Value");
    EXPECT_EQ(items.at(4).first, "]");
}

TEST(StringStreamTest_ItemSplitting, WorksWithFlatMultipleKeyValues) {
    auto items = InputStringStream::split_items(R"({"Multiplier":1,"Unit":"A","Value":4})");
    EXPECT_EQ(items.at(0).first, "Multiplier");
    EXPECT_EQ(items.at(1).first, "1");
    EXPECT_EQ(items.at(2).first, "Unit");
    EXPECT_EQ(items.at(3).first, "A");
    EXPECT_EQ(items.at(4).first, "Value");
    EXPECT_EQ(items.at(5).first, "4");
}


TEST(StringStreamTest_get_item_and_move_to_next, ReturnsFirstTwoItems) {
    InputStringStream ss(TEST_INPUT);

    EXPECT_EQ(ss.get_item_and_move_to_next(), "V2G_Message");
    EXPECT_EQ(ss.get_item_and_move_to_next(), "Header");
}

TEST(StringStreamTest_get_item_and_move_to_next, WorksWithList) {
    InputStringStream ss(TEST_LIST);

    EXPECT_EQ(ss.get_item_and_move_to_next(), "PaymentOption");
    EXPECT_TRUE(ss.is_list_start_move_to_next());
    EXPECT_EQ(ss.get_item_and_move_to_next(), "ExternalPayment");
    EXPECT_TRUE(ss.is_list_end_move_to_next());
}

TEST(StringStreamTest_get_item_and_move_to_next, WorksWithStringsContainingColon) {
    InputStringStream ss(R"({"ProtocolNamespace":"urn:din:70121:2012:MsgDef"})");

    EXPECT_EQ(ss.get_item_and_move_to_next(), "ProtocolNamespace");
    EXPECT_EQ(ss.get_item_and_move_to_next(), "urn:din:70121:2012:MsgDef");
}

TEST(StringStreamTest_get_item_and_move_to_next, WorksWithStringsContainingRectangularBrakets) {
    InputStringStream ss(R"({"ProtocolNamespace":"urn]din[70121"})");

    EXPECT_EQ(ss.get_item_and_move_to_next(), "ProtocolNamespace");
    EXPECT_EQ(ss.get_item_and_move_to_next(), "urn]din[70121");
}

TEST(StringStreamTest_get_item_and_move_to_next, WorksWithStringsContainingCurledBrakets) {
    InputStringStream ss(R"({"ProtocolNamespace":"urn}din{70121"})");

    EXPECT_EQ(ss.get_item_and_move_to_next(), "ProtocolNamespace");
    EXPECT_EQ(ss.get_item_and_move_to_next(), "urn}din{70121");
}

TEST(StringStreamTest_get_item, ReturnsEmptyString_when_at_end) {
    InputStringStream ss(R"({"Item1":"Item2"})");

    EXPECT_EQ(ss.get_item_and_move_to_next(), "Item1");
    EXPECT_EQ(ss.get_item_and_move_to_next(), "Item2");
    EXPECT_EQ(ss.get_item(), "");
}
