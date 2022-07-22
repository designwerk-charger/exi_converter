#include "gtest/gtest.h"

#include "base/json_parser.h"


TEST(JsonParser_parse, ThrowsInvalidArgument_when_InputNotStartingWithCurledBrakets) {
    EXPECT_THROW(JsonParser::parse("no_brakets"), std::invalid_argument);
}

TEST(JsonParser_parse, ContainsSingleKeyAndStringValue) {
    auto item = JsonParser::parse(R"({"Key":"Value"})");
    EXPECT_EQ(item->size(), 1);
    const auto & value = (*item)["Key"]->get_value();
    EXPECT_EQ(value, "Value");
}

TEST(JsonParser_parse, ContainsSingleKeyAndStringValueWithEscapeCharacters) {
    auto item = JsonParser::parse(R"({"V2G_M\"essage":"Value"})");

    const auto & value = (*item)[R"(V2G_M\"essage)"]->get_value();
    EXPECT_EQ(value, "Value");
}

TEST(JsonParser_parse, ContainsSingleKeyAndIntValue) {
    auto item = JsonParser::parse(R"({"Key":12})");

    const auto & value = (*item)["Key"]->get_value();
    EXPECT_EQ(value, "12");
}

TEST(JsonParser_parse, ContainsKeyWithKeyAndStringValue) {
    auto item = JsonParser::parse(R"({"Key1":{"Key2":"Value"}})");
    const auto & obj2 = (*item)["Key1"]->to_object();

    EXPECT_EQ(obj2["Key2"]->get_value(), "Value");
}

TEST(JsonParser_parse, ContainsKeyWithTwoKeysAndStringAndIntValue) {
    auto item = JsonParser::parse(R"({"Key1":{"Key2":"Value1","Key3":33}})");
    const auto & obj = (*item)["Key1"]->to_object();

    EXPECT_EQ(obj["Key2"]->get_value(), "Value1");
    EXPECT_EQ(obj["Key3"]->get_value(), "33");
}

TEST(JsonParser_parse, ContainsTwoKeysUpfront) {
    auto item = JsonParser::parse(R"({"Key1":"Value1","Key3":33})");

    EXPECT_EQ((*item)["Key1"]->get_value(), "Value1");
    EXPECT_EQ((*item)["Key3"]->get_value(), "33");
}

TEST(JsonParser_parse, ContainsKeyWithTwoArrayStrings) {
    auto item = JsonParser::parse(R"({"Key":["Arr1","Arr2"]})");

    const auto & arr = (*item)["Key"]->get_array();

    EXPECT_EQ(arr.at(0)->get_value(), "Arr1");
    EXPECT_EQ(arr.at(1)->get_value(), "Arr2");
}

TEST(JsonParser_parse, ContainsKeyWithOneArrayKeyValue) {
    auto item = JsonParser::parse(R"({"Key":[{"Subkey":"Value"}]})");

    const auto & arr = (*item)["Key"]->get_array();
    const auto & obj = arr[0]->to_object();
    const auto & subkey = obj["Subkey"];

    EXPECT_EQ(subkey->get_value(), "Value");
}

TEST(JsonParser_parse, ContainsKeyWithTwoArrayKeyValue) {
    auto item = JsonParser::parse(R"({"Key":[{"Subkey1":"Value1"},{"Subkey2":44}]})");

    const auto & arr = (*item)["Key"]->get_array();
    const auto & obj0 = arr[0]->to_object();
    const auto & obj1 = arr[1]->to_object();

    EXPECT_EQ(obj0["Subkey1"]->get_value(), "Value1");
    EXPECT_EQ(obj1["Subkey2"]->get_value(), "44");
}

TEST(JsonParser_parse, ContainsKeyWithOneArrayKeyOfTwoKeyValues) {
    auto item = JsonParser::parse(R"({"Key":[{"Subkey1":"Value1","Subkey2":44}]})");

    const auto & arr = (*item)["Key"]->get_array();
    const auto & obj0 = arr[0]->to_object();

    EXPECT_EQ(obj0["Subkey1"]->get_value(), "Value1");
    EXPECT_EQ(obj0["Subkey2"]->get_value(), "44");
}

TEST(JsonParser_parse, WorksWithEmptyKey) {
    auto item = JsonParser::parse(R"({"SessionStopReq":{}})");

    const auto & empty_val = (*item)["SessionStopReq"];
    EXPECT_TRUE(empty_val->is_empty());
}

TEST(JsonParser_parse, WorksWithEmptyArray) {
    auto item = JsonParser::parse(R"({"SessionStopReq":[]})");

    const auto & empty_val = (*item)["SessionStopReq"];
    EXPECT_TRUE(empty_val->is_empty());
}

TEST(JsonParser_parse, WorksWithStringsContainingColon) {
    auto item = JsonParser::parse(R"({"ProtocolNamespace":"urn:din:70121:2012:MsgDef"})");

    EXPECT_EQ((*item)["ProtocolNamespace"]->get_value(), "urn:din:70121:2012:MsgDef");
}

TEST(JsonParser_parse, WorksWithStringsContainingRectangularBrakets) {
    auto item = JsonParser::parse(R"({"ProtocolNamespace":"urn]din[70121"})");

    EXPECT_EQ((*item)["ProtocolNamespace"]->get_value(), "urn]din[70121");
}

TEST(JsonParser_parse, WorksWithStringsContainingCurledBrakets) {
    auto item = JsonParser::parse(R"({"ProtocolNamespace":"urn}din{70121"})");

    EXPECT_EQ((*item)["ProtocolNamespace"]->get_value(), "urn}din{70121");
}

TEST(JsonParser_parse, NullCharacterTest) {
    auto item = JsonParser::parse(R"({"Nullchar:":""})");

    EXPECT_EQ((*item)["Nullchar:\0"]->get_value(), "\0");
}

TEST(JsonParser_parseRealWorld, WorksWithAComplexConstruct1) {
    auto item = JsonParser::parse(R"({"CanonicalizationMethod":{"Algorithm":"http://www.w3.org/TR/canonical-exi/"},"SignatureMethod":{"Algorithm":"http://www.w3.org/2001/04/xmldsig-more#ecdsa-sha256"},"Reference":[{"URI":"#id1","Transforms":{"Transform":[{"Algorithm":"http://www.w3.org/TR/canonical-exi/"}]},"DigestMethod":{"Algorithm":"http://www.w3.org/2001/04/xmlenc#sha256"},"DigestValue":"FOPYeuJCS2wwX9XxiiT+Ly9ofNZY1Zu+UafOP1zwhvA="}]})");

    EXPECT_EQ((*item)["CanonicalizationMethod"]->to_object()["Algorithm"]->get_value(),
              "http://www.w3.org/TR/canonical-exi/");
    EXPECT_EQ((*item)["SignatureMethod"]->to_object()["Algorithm"]->get_value(),
              "http://www.w3.org/2001/04/xmldsig-more#ecdsa-sha256");

    auto ref0 = (*item)["Reference"]->get_array()[0]->to_object();
    EXPECT_EQ(ref0["URI"]->get_value(), "#id1");
    EXPECT_EQ(ref0["DigestMethod"]->to_object()["Algorithm"]->get_value(), "http://www.w3.org/2001/04/xmlenc#sha256");
    EXPECT_EQ(ref0["DigestValue"]->get_value(), "FOPYeuJCS2wwX9XxiiT+Ly9ofNZY1Zu+UafOP1zwhvA=");

    auto transform = ref0["Transforms"]->to_object()["Transform"]->get_array();
    EXPECT_EQ(transform[0]->to_object()["Algorithm"]->get_value(), "http://www.w3.org/TR/canonical-exi/");
}

TEST(JsonParser_parseRealWorld, WorksWithAComplexConstruct2) {
    auto item = JsonParser::parse(R"({"SalesTariffEntry":[{"RelativeTimeInterval":44,"EPriceLevel":1},{"RelativeTimeInterval":55,"EPriceLevel":2}]})");

    auto arr = (*item)["SalesTariffEntry"]->get_array();

    EXPECT_EQ(arr[0]->to_object()["RelativeTimeInterval"]->get_value(), "44");
    EXPECT_EQ(arr[0]->to_object()["EPriceLevel"]->get_value(), "1");
    EXPECT_EQ(arr[1]->to_object()["RelativeTimeInterval"]->get_value(), "55");
    EXPECT_EQ(arr[1]->to_object()["EPriceLevel"]->get_value(), "2");
}

TEST(JsonParser_parseRealWorld, WorksWithAComplexConstruct3) {
    auto item = JsonParser::parse(R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"SessionSetupRes":{"ResponseCode":"OK_NewSessionEstablished","EVSEID":"UK123E1234","EVSETimeStamp":1652790080}}}})");

    EXPECT_EQ((*item)["V2G_Message"]->to_object()["Header"]->to_object()["SessionID"]->get_value(), "37DA98F73608903E");
    EXPECT_EQ((*item)["V2G_Message"]->to_object()["Body"]->to_object()["SessionSetupRes"] \
    ->to_object()["ResponseCode"]->get_value(), "OK_NewSessionEstablished");
}
