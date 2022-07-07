#include "gtest/gtest.h"
#include "integration_test/type_conversion.h"
#include "library.h"


TestDataContainer const APP_PROTOCOL_TEST_DATA[] = {
        TestDataContainer(
                "supportedAppProtocolReq",
                "8000dbab9371d3234b71d1b981899189d191818991d26b9b3a232b30020000040001d75726e3a69736f3a31353131383a323a323031333a4d73674465660040000100803ceae4dc74d2e6de74e6e8c874d2e6de74626a626270745a646074828600400003021",  // NOLINT
                R"({"supportedAppProtocolReq":{"AppProtocol":[{"ProtocolNamespace":"urn:din:70121:2012:MsgDef","VersionNumberMajor":2,"VersionNumberMinor":0,"SchemaID":1,"Priority":1},{"ProtocolNamespace":"urn:iso:15118:2:2013:MsgDef","VersionNumberMajor":2,"VersionNumberMinor":0,"SchemaID":2,"Priority":2},{"ProtocolNamespace":"urn:iso:std:iso:15118:-20:AC","VersionNumberMajor":1,"VersionNumberMinor":0,"SchemaID":3,"Priority":3}]}})"),
        TestDataContainer(
                "supportedAppProtocolRes",
                "80400080",
                R"({"supportedAppProtocolRes":{"ResponseCode":"OK_SuccessfulNegotiation","SchemaID":2}})"),
        TestDataContainer(
                "supportedAppProtocolReq_With0Character_CausingErrorInOldCcsHandler",
                "8000e3ab9371d3234b71d1b981899189d191818991d26b9b3a232b3000020000280401e75726e3a69736f3a31353131383a323a323031333a4d7367446566000040000a00080",   // NOLINT
                R"({"supportedAppProtocolReq":{"AppProtocol":[{"ProtocolNamespace":"urn:din:70121:2012:MsgDef","VersionNumberMajor":2,"VersionNumberMinor":0,"SchemaID":10,"Priority":2},{"ProtocolNamespace":"urn:iso:15118:2:2013:MsgDef","VersionNumberMajor":2,"VersionNumberMinor":0,"SchemaID":20,"Priority":1}]}})")
};

/* ************************************
 * Decoding
 * ********************************* */
class DecodingAppProtocol :public ::testing::TestWithParam<TestDataContainer> { };

TEST_P(DecodingAppProtocol, WithExiDecoder) {
    TestDataContainer test_data = GetParam();
    ASSERT_EQ(run_decoding(test_data, "urn:iso:15118:2:2010:AppProtocol"), test_data.json_str);
}

INSTANTIATE_TEST_CASE_P(
        RequestResponseMessages,
        DecodingAppProtocol,
        ::testing::ValuesIn(APP_PROTOCOL_TEST_DATA));
