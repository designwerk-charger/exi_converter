#include "gtest/gtest.h"

#include "base/json_parser.h"


TEST(JsonParser_parse, ThrowsInvalidArgument_when_InputNotStartingWithCurledBrakets) {
    EXPECT_THROW(JsonParser::parse("no_brakets"), std::invalid_argument);
}

TEST(JsonParser_parse, ContainsSingleKeyAndStringValue) {
    JObject item = JsonParser::parse(R"({"Key":"Value"})");
    EXPECT_EQ(item.size(), 1);
    const auto & value = item["Key"]->get_value();
    EXPECT_EQ(value, "Value");
}

TEST(JsonParser_parse, ContainsSingleKeyAndStringValueWithEscapeCharacters) {
    JObject item = JsonParser::parse(R"({"V2G_M\"essage":"Value"})");

    const auto & value = item[R"(V2G_M\"essage)"]->get_value();
    EXPECT_EQ(value, "Value");
}

TEST(JsonParser_parse, ContainsSingleKeyAndIntValue) {
    JObject item = JsonParser::parse(R"({"Key":12})");

    const auto & value = item["Key"]->get_value();
    EXPECT_EQ(value, "12");
}

TEST(JsonParser_parse, ContainsKeyWithKeyAndStringValue) {
    JObject item = JsonParser::parse(R"({"Key1":{"Key2":"Value"}})");
    const auto & obj2 = item["Key1"]->to_object();

    EXPECT_EQ(obj2["Key2"]->get_value(), "Value");
}

TEST(JsonParser_parse, ContainsKeyWithTwoKeysAndStringAndIntValue) {
    JObject item = JsonParser::parse(R"({"Key1":{"Key2":"Value1","Key3":33}})");
    const auto & obj = item["Key1"]->to_object();

    EXPECT_EQ(obj["Key2"]->get_value(), "Value1");
    EXPECT_EQ(obj["Key3"]->get_value(), "33");
}

TEST(JsonParser_parse, ContainsTwoKeysUpfront) {
    JObject item = JsonParser::parse(R"({"Key1":"Value1","Key3":33})");

    EXPECT_EQ(item["Key1"]->get_value(), "Value1");
    EXPECT_EQ(item["Key3"]->get_value(), "33");
}

TEST(JsonParser_parse, ContainsKeyWithTwoArrayStrings) {
    JObject item = JsonParser::parse(R"({"Key":["Arr1","Arr2"]})");

    const auto & arr = item["Key"]->get_array();

    EXPECT_EQ(arr.at(0)->get_value(), "Arr1");
    EXPECT_EQ(arr.at(1)->get_value(), "Arr2");
}

TEST(JsonParser_parse, ContainsKeyWithOneArrayKeyValue) {
    JObject item = JsonParser::parse(R"({"Key":[{"Subkey":"Value"}]})");

    const auto & arr = item["Key"]->get_array();
    const auto & obj = arr[0]->to_object();
    const auto & subkey = obj["Subkey"];

    EXPECT_EQ(subkey->get_value(), "Value");
}

TEST(JsonParser_parse, ContainsKeyWithTwoArrayKeyValue) {
    JObject item = JsonParser::parse(R"({"Key":[{"Subkey1":"Value1"},{"Subkey2":44}]})");

    const auto & arr = item["Key"]->get_array();
    const auto & obj0 = arr[0]->to_object();
    const auto & obj1 = arr[1]->to_object();

    EXPECT_EQ(obj0["Subkey1"]->get_value(), "Value1");
    EXPECT_EQ(obj1["Subkey2"]->get_value(), "44");
}

TEST(JsonParser_parse, ContainsKeyWithOneArrayKeyOfTwoKeyValues) {
    JObject item = JsonParser::parse(R"({"Key":[{"Subkey1":"Value1","Subkey2":44}]})");

    const auto & arr = item["Key"]->get_array();
    const auto & obj0 = arr[0]->to_object();

    EXPECT_EQ(obj0["Subkey1"]->get_value(), "Value1");
    EXPECT_EQ(obj0["Subkey2"]->get_value(), "44");
}

TEST(JsonParser_parse, WorksWithEmptyKey) {
    JObject item = JsonParser::parse(R"({"SessionStopReq":{}})");

    const auto & empty_val = item["SessionStopReq"];
    EXPECT_TRUE(empty_val->is_empty());
}

TEST(JsonParser_parse, WorksWithStringsContainingColon) {
    JObject item = JsonParser::parse(R"({"ProtocolNamespace":"urn:din:70121:2012:MsgDef"})");

    EXPECT_EQ(item["ProtocolNamespace"]->get_value(), "urn:din:70121:2012:MsgDef");
}

TEST(JsonParser_parse, WorksWithStringsContainingRectangularBrakets) {
    JObject item = JsonParser::parse(R"({"ProtocolNamespace":"urn]din[70121"})");

    EXPECT_EQ(item["ProtocolNamespace"]->get_value(), "urn]din[70121");
}

TEST(JsonParser_parse, WorksWithStringsContainingCurledBrakets) {
    JObject item = JsonParser::parse(R"({"ProtocolNamespace":"urn}din{70121"})");

    EXPECT_EQ(item["ProtocolNamespace"]->get_value(), "urn}din{70121");
}

TEST(JsonParser_parse, NullCharacterTest) {
    JObject item = JsonParser::parse(R"({"Nullchar:":""})");

    EXPECT_EQ(item["Nullchar:\0"]->get_value(), "\0");
}

TEST(JsonParser_parseRealWorld, WorksWithAComplexConstruct1) {
    JObject item = JsonParser::parse(R"({"CanonicalizationMethod":{"Algorithm":"http://www.w3.org/TR/canonical-exi/"},"SignatureMethod":{"Algorithm":"http://www.w3.org/2001/04/xmldsig-more#ecdsa-sha256"},"Reference":[{"URI":"#id1","Transforms":{"Transform":[{"Algorithm":"http://www.w3.org/TR/canonical-exi/"}]},"DigestMethod":{"Algorithm":"http://www.w3.org/2001/04/xmlenc#sha256"},"DigestValue":"FOPYeuJCS2wwX9XxiiT+Ly9ofNZY1Zu+UafOP1zwhvA="}]})");

    EXPECT_EQ(item["CanonicalizationMethod"]->to_object()["Algorithm"]->get_value(),
              "http://www.w3.org/TR/canonical-exi/");
    EXPECT_EQ(item["SignatureMethod"]->to_object()["Algorithm"]->get_value(),
              "http://www.w3.org/2001/04/xmldsig-more#ecdsa-sha256");

    auto ref0 = item["Reference"]->get_array()[0]->to_object();
    EXPECT_EQ(ref0["URI"]->get_value(), "#id1");
//    EXPECT_EQ(ref0["DigestMethod"]->to_object()["Algorithm"]->get_value(), "http://www.w3.org/2001/04/xmlenc#sha256");
//    EXPECT_EQ(ref0["DigestValue"]->get_value(), "FOPYeuJCS2wwX9XxiiT+Ly9ofNZY1Zu+UafOP1zwhvA=");

    auto transform = ref0["Transforms"]->to_object()["Transform"]->get_array();
    EXPECT_EQ(transform[0]->to_object()["Algorithm"]->get_value(), "http://www.w3.org/TR/canonical-exi/");
}

TEST(JsonParser_parseRealWorld, WorksWithAComplexConstruct2) {
    JObject item = JsonParser::parse(R"({"SalesTariffEntry":[{"RelativeTimeInterval":44,"EPriceLevel":1},{"RelativeTimeInterval":55,"EPriceLevel":2}]})");

    auto arr = item["SalesTariffEntry"]->get_array();

    EXPECT_EQ(arr[0]->to_object()["RelativeTimeInterval"]->get_value(), "44");
    EXPECT_EQ(arr[1]->to_object()["RelativeTimeInterval"]->get_value(), "55");
    /*
    assert_item_equality(items, {"SalesTariffEntry", "[", "RelativeTimeInterval", "start", "0",
                                 "EPriceLevel", "1", "RelativeTimeInterval", "start", "1801", "duration", "1799",
                                 "EPriceLevel", "2", "]"});
                                 */
}

