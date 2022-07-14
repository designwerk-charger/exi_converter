#include "gtest/gtest.h"
#include "integration_test/type_conversion.h"
#include "library.h"


TestDataContainer const ISO15118_2_RESPONSE_TEST_DATA[] = {
        TestDataContainer(
                "SessionSetupRes",
                "8098020df6a63dcd82240f91e02031552cc4c8cd14c4c8ccd01814d1d280c0",
                R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"SessionSetupRes":{"ResponseCode":"OK_NewSessionEstablished","EVSEID":"UK123E1234","EVSETimeStamp":1652790080}}}})"),
        TestDataContainer(
                "ServiceDiscoveryRes",
                "8098020df6a63dcd82240f91c0012004041050d7d110d7d0da185c99da5b99c0406480",
                R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"ServiceDiscoveryRes":{"ResponseCode":"OK","PaymentOptionList":{"PaymentOption":["ExternalPayment"]},"ChargeService":{"ServiceID":1,"ServiceName":"AC_DC_Charging","ServiceCategory":"EVCharging","FreeService":false,"SupportedEnergyTransferMode":{"EnergyTransferMode":["DC_extended"]}}}}}})"),
        TestDataContainer(
                "PaymentServiceSelectionRes",
                "8098020df6a63dcd82240f914000",
                R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"PaymentServiceSelectionRes":{"ResponseCode":"OK"}}}})"),
        TestDataContainer(
                "AuthorizationRes",
                "8098020df6a63dcd82240f90100000",
                R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"AuthorizationRes":{"ResponseCode":"OK","EVSEProcessing":"Finished"}}}})"),
        TestDataContainer(
                "ChargeParameterDiscoveryRes",
                "8098020df6a63dcd82240f8a895a1d1d1c0e8bcbddddddcb9dcccb9bdc99cbd5148bd8d85b9bdb9a58d85b0b595e1a4bd0d5a1d1d1c0e8bcbddddddcb9dcccb9bdc99cbcc8c0c0c4bcc0d0bde1b5b191cda59cb5b5bdc9948d958d91cd84b5cda184c8d4d910311b4b218812b43a3a381d1797bbbbbb973b999737b93397aa2917b1b0b737b734b1b0b616b2bc3497a429687474703a2f2f7777772e77332e6f72672f323030312f30342f786d6c656e632373686132353642014e3d87ae2424b6c305fd5f18a24fe2f2f687cd658d59bbe51a7ce3f5cf086f0128c60880440ad45013e7aa4744a6e6453cad9eb6a96ed1b07e44286bce1ae4ce99f8a8e5132044038f219bbed1e06b94024a1798bed853ff191b6c647027f019db7544172e5b01682800000000000901c03143e154200ad2c862049010000400902243821c380045286400102081802020a1cc020820020206004082002090300410", // NOLINT
                R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E","Signature":{"SignedInfo":{"CanonicalizationMethod":{"Algorithm":"http://www.w3.org/TR/canonical-exi/"},"SignatureMethod":{"Algorithm":"http://www.w3.org/2001/04/xmldsig-more#ecdsa-sha256"},"Reference":[{"URI":"#id1","Transforms":{"Transform":[{"Algorithm":"http://www.w3.org/TR/canonical-exi/"}]},"DigestMethod":{"Algorithm":"http://www.w3.org/2001/04/xmlenc#sha256"},"DigestValue":"FOPYeuJCS2wwX9XxiiT+Ly9ofNZY1Zu+UafOP1zwhvA="}]},"SignatureValue":{"value":"MEQCIFaigJ89UjolNzIp5Wz1tUt2jYPyIUNecNcmdM/FRyiZAiAceQzd9o8DXKASULzF9sKf+MjbYyOBP4DO26oguXLYCw=="}}},"Body":{"ChargeParameterDiscoveryRes":{"ResponseCode":"OK","EVSEProcessing":"Finished","SAScheduleList":{"SAScheduleTuple":[{"SAScheduleTupleID":1,"PMaxSchedule":{"PMaxScheduleEntry":[{"RelativeTimeInterval":{"start":0,"duration":3600},"PMax":{"Multiplier":0,"Unit":"W","Value":11000}}]},"SalesTariff":{"Id":"id1","SalesTariffID":10,"NumEPriceLevels":2,"SalesTariffEntry":[{"RelativeTimeInterval":{"start":0},"EPriceLevel":1},{"RelativeTimeInterval":{"start":1801,"duration":1799},"EPriceLevel":2}]}}]},"DC_EVSEChargeParameter":{"DC_EVSEStatus":{"NotificationMaxDelay":100,"EVSENotification":"None","EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready"},"EVSEMaximumCurrentLimit":{"Multiplier":1,"Unit":"A","Value":4},"EVSEMaximumPowerLimit":{"Multiplier":1,"Unit":"W","Value":230},"EVSEMaximumVoltageLimit":{"Multiplier":1,"Unit":"V","Value":4},"EVSEMinimumCurrentLimit":{"Multiplier":1,"Unit":"A","Value":2},"EVSEMinimumVoltageLimit":{"Multiplier":1,"Unit":"V","Value":4},"EVSEPeakCurrentRipple":{"Multiplier":1,"Unit":"A","Value":4}}}}}})"), // NOLINT
        TestDataContainer(
                "CableCheckRes",
                "8098020df6a63dcd82240f9040000000204000",
                R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"CableCheckRes":{"ResponseCode":"OK","DC_EVSEStatus":{"NotificationMaxDelay":0,"EVSENotification":"None","EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready"},"EVSEProcessing":"Finished"}}}})"),
        TestDataContainer(
                "PreChargeRes",
                "8098020df6a63dcd82240f91800000002040c40e601000",
                R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"PreChargeRes":{"ResponseCode":"OK","DC_EVSEStatus":{"NotificationMaxDelay":0,"EVSENotification":"None","EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready"},"EVSEPresentVoltage":{"Multiplier":0,"Unit":"V","Value":230}}}}})"),
        TestDataContainer(
                "PowerDeliveryRes",
                "8098020df6a63dcd82240f91600400001020",
                R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"PowerDeliveryRes":{"ResponseCode":"OK","DC_EVSEStatus":{"NotificationMaxDelay":0,"EVSENotification":"None","EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready"}}}}})"),
        TestDataContainer(
                "CurrentDemandRes",
                "8098020df6a63dcd82240f90e00000002040c40e601030c0042220310360100618560101050e80700c554b3132334531323334000400", // NOLINT
                R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"CurrentDemandRes":{"ResponseCode":"OK","DC_EVSEStatus":{"NotificationMaxDelay":0,"EVSENotification":"None","EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready"},"EVSEPresentVoltage":{"Multiplier":0,"Unit":"V","Value":230},"EVSEPresentCurrent":{"Multiplier":0,"Unit":"A","Value":1},"EVSECurrentLimitAchieved":true,"EVSEVoltageLimitAchieved":true,"EVSEPowerLimitAchieved":true,"EVSEMaximumVoltageLimit":{"Multiplier":0,"Unit":"V","Value":600},"EVSEMaximumCurrentLimit":{"Multiplier":0,"Unit":"A","Value":300},"EVSEMaximumPowerLimit":{"Multiplier":1,"Unit":"W","Value":1000},"EVSEID":"UK123E1234","SAScheduleTupleID":1,"ReceiptRequired":false}}}})"),
        TestDataContainer(
                "WeldingDetectionRes",
                "8098020df6a63dcd82240f92200000002040c40e601000",
                R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"WeldingDetectionRes":{"ResponseCode":"OK","DC_EVSEStatus":{"NotificationMaxDelay":0,"EVSENotification":"None","EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready"},"EVSEPresentVoltage":{"Multiplier":0,"Unit":"V","Value":230}}}}})"),
        TestDataContainer(
                "SessionStopRes",
                "8098020df6a63dcd82240f920000",
                R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"SessionStopRes":{"ResponseCode":"OK"}}}})")
};

TestDataContainer const ISO15118_2_REQUEST_TEST_DATA[] = {
        TestDataContainer(
                "ServiceDiscoveryReq",
                "8098020df6a63dcd82240f91b8",
                R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"ServiceDiscoveryReq":{}}}})"),
        TestDataContainer(
                "PaymentServiceSelectionReq",
                "8098020df6a63dcd82240f9132001280",
                R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"PaymentServiceSelectionReq":{"SelectedPaymentOption":"ExternalPayment","SelectedServiceList":{"SelectedService":[{"ServiceID":1}]}}}}})"),
        TestDataContainer(
                "AuthorizationReq",
                "8098020df6a63dcd82240f9008",
                R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"AuthorizationReq":{}}}})"),
        TestDataContainer(
                "ChargeParameterDiscoveryReq",
                "8098020df6a63dcd82240f9094c8000800f0003080fa01020a1807c082014008306c1b00830781702d0500",
                // 0x00 removed from end
                R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"ChargeParameterDiscoveryReq":{"RequestedEnergyTransferMode":"DC_extended","DC_EVChargeParameter":{"DepartureTime":0,"DC_EVStatus":{"EVReady":true,"EVErrorCode":"NO_ERROR","EVRESSSOC":60},"EVMaximumCurrentLimit":{"Multiplier":-3,"Unit":"A","Value":32000},"EVMaximumPowerLimit":{"Multiplier":1,"Unit":"W","Value":8000},"EVMaximumVoltageLimit":{"Multiplier":1,"Unit":"V","Value":40},"EVEnergyCapacity":{"Multiplier":1,"Unit":"Wh","Value":7000},"EVEnergyRequest":{"Multiplier":1,"Unit":"Wh","Value":6000},"FullSOC":90,"BulkSOC":80}}}}})"),
        TestDataContainer(
                "CableCheckReq",
                "8098020df6a63dcd82240f9031001e00",
                R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"CableCheckReq":{"DC_EVStatus":{"EVReady":true,"EVErrorCode":"NO_ERROR","EVRESSSOC":60}}}}})"),
        TestDataContainer(
                "PreChargeReq",
                "8098020df6a63dcd82240f9171001e018812006061800800",
                R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"PreChargeReq":{"DC_EVStatus":{"EVReady":true,"EVErrorCode":"NO_ERROR","EVRESSSOC":60},"EVTargetVoltage":{"Multiplier":0,"Unit":"V","Value":400},"EVTargetCurrent":{"Multiplier":0,"Unit":"A","Value":1}}}}})"),
        TestDataContainer(
                "CurrentDemandReq",
                "8098020df6a63dcd82240f90d1001e01860021006101f4020414300f80003081900610280188120060",
                R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"CurrentDemandReq":{"DC_EVStatus":{"EVReady":true,"EVErrorCode":"NO_ERROR","EVRESSSOC":60},"EVTargetCurrent":{"Multiplier":0,"Unit":"A","Value":1},"EVMaximumCurrentLimit":{"Multiplier":-3,"Unit":"A","Value":32000},"EVMaximumPowerLimit":{"Multiplier":1,"Unit":"W","Value":8000},"BulkChargingComplete":false,"ChargingComplete":false,"RemainingTimeToFullSoC":{"Multiplier":0,"Unit":"s","Value":100},"RemainingTimeToBulkSoC":{"Multiplier":0,"Unit":"s","Value":80},"EVTargetVoltage":{"Multiplier":0,"Unit":"V","Value":400}}}}})"),
        TestDataContainer(
                "PowerDeliveryReq",
                "8098020df6a63dcd82240f915000022003c140",
                R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"PowerDeliveryReq":{"ChargeProgress":"Start","SAScheduleTupleID":1,"DC_EVPowerDeliveryParameter":{"DC_EVStatus":{"EVReady":true,"EVErrorCode":"NO_ERROR","EVRESSSOC":60},"ChargingComplete":true}}}}})"),
        TestDataContainer(
                "WeldingDetectionReq",
                "8098020df6a63dcd82240f9211001e00",
                R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"WeldingDetectionReq":{"DC_EVStatus":{"EVReady":true,"EVErrorCode":"NO_ERROR","EVRESSSOC":60}}}}})"),
        TestDataContainer(
                "SessionStopReq",
                "8098020df6a63dcd82240f91f000",
                R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"SessionStopReq":{"ChargingSession":"Terminate"}}}})")
};

/* ************************************
 * Decoding
 * ********************************* */
class DecodingISO15118_2 :public ::testing::TestWithParam<TestDataContainer> { };

TEST_P(DecodingISO15118_2, WithExiDecoder) {
    TestDataContainer test_data = GetParam();
    ASSERT_EQ(run_decoding(test_data, "urn:iso:15118:2:2013:MsgDef"), test_data.json_str);
}

INSTANTIATE_TEST_CASE_P(
        ResponseMessages,
        DecodingISO15118_2,
        ::testing::ValuesIn(ISO15118_2_RESPONSE_TEST_DATA)
);

INSTANTIATE_TEST_CASE_P(
        RequestMessages,
        DecodingISO15118_2,
        ::testing::ValuesIn(ISO15118_2_REQUEST_TEST_DATA)
);


/* ************************************
 * Specific test messages
 * ********************************* */
TEST(ISO15118_2, SpecificMessageDecoding) {
    auto ISO_MSG = TestDataContainer(
            "SessionStopReq",
            "8098020df6a63dcd82240f91f000",
            R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"SessionStopReq":{"ChargingSession":"Terminate"}}}})");  // NOLINT
    ASSERT_EQ(run_decoding(ISO_MSG, "urn:iso:15118:2:2013:MsgDef"), ISO_MSG.json_str);
}

TEST(ISO15118_2, SpecificMessageEncoding) {
    auto ISO_MSG = TestDataContainer(
            "SessionStopReq",
            "8098020df6a63dcd82240f91f000",
            R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"SessionStopReq":{"ChargingSession":"Terminate"}}}})");  // NOLINT
    ASSERT_EQ(run_encoding(ISO_MSG, "urn:iso:15118:2:2013:MsgDef"), ISO_MSG.hex_str);
}
