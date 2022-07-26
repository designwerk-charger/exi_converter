#include "gtest/gtest.h"
#include "integration_test/type_conversion.h"
#include "library.h"


TestDataContainer const DIN_SPEC_RESPONSE_TEST_DATA[] = {
        TestDataContainer(
                "SessionSetupRes",
                "809a022fe93b3d1267a2c991e0201526a2698d801f7bda5280c0",
                R"({"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"SessionSetupRes":{"ResponseCode":"OK_NewSessionEstablished","EVSEID":"49A89A6360","DateTimeNow":1653911419}}}})"),
        TestDataContainer(
                "ServiceDiscoveryRes",
                "809a022fe93b3d1267a2c991a00120024100c4",
                R"({"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"ServiceDiscoveryRes":{"ResponseCode":"OK","PaymentOptions":{"PaymentOption":["ExternalPayment"]},"ChargeService":{"ServiceTag":{"ServiceID":1,"ServiceCategory":"EVCharging"},"FreeService":false,"EnergyTransferType":"DC_extended"}}}}})"),
        TestDataContainer(
                "ServicePaymentSelectionRes",
                "809a022fe93b3d1267a2c991c000",
                R"({"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"ServicePaymentSelectionRes":{"ResponseCode":"OK"}}}})"),
        TestDataContainer(
                "ContractAuthenticationRes",
                "809a022fe93b3d1267a2c990c00000",
                R"({"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"ContractAuthenticationRes":{"ResponseCode":"OK","EVSEProcessing":"Finished"}}}})"),
        TestDataContainer(
                "ChargeParameterDiscoveryRes",
                "809a022fe93b3d1267a2c990800000040000000480e03200455040864002018020101c398041014010100c00810140104806008200",  // NOLINT
                R"({"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"ChargeParameterDiscoveryRes":{"ResponseCode":"OK","EVSEProcessing":"Finished","SAScheduleList":{"SAScheduleTuple":[{"SAScheduleTupleID":1,"PMaxSchedule":{"PMaxScheduleID":0,"PMaxScheduleEntry":[{"RelativeTimeInterval":{"start":0,"duration":3600},"PMax":200}]}}]},"DC_EVSEChargeParameter":{"DC_EVSEStatus":{"EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready","NotificationMaxDelay":100,"EVSENotification":"None"},"EVSEMaximumCurrentLimit":{"Multiplier":1,"Unit":"A","Value":4},"EVSEMaximumPowerLimit":{"Multiplier":1,"Unit":"W","Value":230},"EVSEMaximumVoltageLimit":{"Multiplier":1,"Unit":"V","Value":4},"EVSEMinimumCurrentLimit":{"Multiplier":1,"Unit":"A","Value":2},"EVSEMinimumVoltageLimit":{"Multiplier":1,"Unit":"V","Value":4},"EVSEPeakCurrentRipple":{"Multiplier":1,"Unit":"A","Value":4}}}}}})"),
        TestDataContainer(
                "CableCheckRes",
                "809a022fe93b3d1267a2c99020004080000000",
                R"({"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"CableCheckRes":{"ResponseCode":"OK","DC_EVSEStatus":{"EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready","NotificationMaxDelay":0,"EVSENotification":"None"},"EVSEProcessing":"Finished"}}}})"),
        TestDataContainer(
                "PreChargeRes",
                "809a022fe93b3d1267a2c9916000408000018287300800",
                R"({"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"PreChargeRes":{"ResponseCode":"OK","DC_EVSEStatus":{"EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready","NotificationMaxDelay":0,"EVSENotification":"None"},"EVSEPresentVoltage":{"Multiplier":0,"Unit":"V","Value":230}}}}})"),
        TestDataContainer(
                "PowerDeliveryRes",
                "809a022fe93b3d1267a2c991400420400000",
                R"({"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"PowerDeliveryRes":{"ResponseCode":"OK","DC_EVSEStatus":{"EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready","NotificationMaxDelay":0,"EVSENotification":"None"}}}}})"),
        TestDataContainer(
                "CurrentDemandRes",
                "809a022fe93b3d1267a2c990e00040800001828730081818008044c0",
                R"({"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"CurrentDemandRes":{"ResponseCode":"OK","DC_EVSEStatus":{"EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready","NotificationMaxDelay":0,"EVSENotification":"None"},"EVSEPresentVoltage":{"Multiplier":0,"Unit":"V","Value":230},"EVSEPresentCurrent":{"Multiplier":0,"Unit":"A","Value":1},"EVSECurrentLimitAchieved":false,"EVSEVoltageLimitAchieved":true,"EVSEPowerLimitAchieved":true}}}})"),
        TestDataContainer(
                "WeldingDetectionRes",
                "809a022fe93b3d1267a2c9922000408000018287300800",
                R"({"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"WeldingDetectionRes":{"ResponseCode":"OK","DC_EVSEStatus":{"EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready","NotificationMaxDelay":0,"EVSENotification":"None"},"EVSEPresentVoltage":{"Multiplier":0,"Unit":"V","Value":230}}}}})"),
        TestDataContainer(
                "SessionStopRes",
                "809a022fe93b3d1267a2c9920000",
                R"({"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"SessionStopRes":{"ResponseCode":"OK"}}}})"
        )
};

TestDataContainer const DIN_SPEC_REQUEST_TEST_DATA[] = {
        TestDataContainer(
                "SessionSetupReq",
                "809a004011d018038133bc642c00",
                R"({"V2G_Message":{"Header":{"SessionID":"00"},"Body":{"SessionSetupReq":{"EVCCID":"00E04CEF190B"}}}})"),
        TestDataContainer(
                "ServiceDiscoveryReq",
                "809a022fe93b3d1267a2c99198",
                R"({"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"ServiceDiscoveryReq":{}}}})"),
        TestDataContainer(
                "ServicePaymentSelectionReq",
                "809a022fe93b3d1267a2c991b2001280",
                R"({"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"ServicePaymentSelectionReq":{"SelectedPaymentOption":"ExternalPayment","SelectedServiceList":{"SelectedService":[{"ServiceID":1}]}}}}})"),
        TestDataContainer(
                "ContractAuthenticationReq",
                "809a022fe93b3d1267a2c990b8",
                R"({"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"ContractAuthenticationReq":{}}}})"),
        TestDataContainer(
                "ChargeParameterDiscoveryReq",
                "809a022fe93b3d1267a2c9907191400f0000c203e804480a050400",
                R"({"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"ChargeParameterDiscoveryReq":{"EVRequestedEnergyTransferType":"DC_extended","DC_EVChargeParameter":{"DC_EVStatus":{"EVReady":true,"EVErrorCode":"NO_ERROR","EVRESSSOC":60},"EVMaximumCurrentLimit":{"Multiplier":-3,"Unit":"A","Value":32000},"EVMaximumVoltageLimit":{"Multiplier":1,"Unit":"V","Value":40}}}}}})"),
        TestDataContainer(
                "CableCheckReq",
                "809a022fe93b3d1267a2c99011400f00",
                R"({"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"CableCheckReq":{"DC_EVStatus":{"EVReady":true,"EVErrorCode":"NO_ERROR","EVRESSSOC":60}}}}})"),
        TestDataContainer(
                "PreChargeReq",
                "809a022fe93b3d1267a2c99151400f00c142400c0c0c0040",
                R"({"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"PreChargeReq":{"DC_EVStatus":{"EVReady":true,"EVErrorCode":"NO_ERROR","EVRESSSOC":60},"EVTargetVoltage":{"Multiplier":0,"Unit":"V","Value":400},"EVTargetCurrent":{"Multiplier":0,"Unit":"A","Value":1}}}}})"),
        TestDataContainer(
                "PowerDeliveryReq",
                "809a022fe93b3d1267a2c9913222801e0a00",
                R"({"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"PowerDeliveryReq":{"ReadyToChargeState":true,"DC_EVPowerDeliveryParameter":{"DC_EVStatus":{"EVReady":true,"EVErrorCode":"NO_ERROR","EVRESSSOC":60},"ChargingComplete":true}}}}})"),
        TestDataContainer(
                "CurrentDemandReq",
                "809a022fe93b3d1267a2c990d1400f00c0c00404050280000c203e804080e1807c00018103200c081400c142400c00",  // NOLINT
                R"({"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"CurrentDemandReq":{"DC_EVStatus":{"EVReady":true,"EVErrorCode":"NO_ERROR","EVRESSSOC":60},"EVTargetCurrent":{"Multiplier":0,"Unit":"A","Value":1},"EVMaximumVoltageLimit":{"Multiplier":1,"Unit":"V","Value":40},"EVMaximumCurrentLimit":{"Multiplier":-3,"Unit":"A","Value":32000},"EVMaximumPowerLimit":{"Multiplier":1,"Unit":"W","Value":8000},"BulkChargingComplete":false,"ChargingComplete":false,"RemainingTimeToFullSoC":{"Multiplier":0,"Unit":"s","Value":100},"RemainingTimeToBulkSoC":{"Multiplier":0,"Unit":"s","Value":80},"EVTargetVoltage":{"Multiplier":0,"Unit":"V","Value":400}}}}})"),
        TestDataContainer(
                "WeldingDetectionReq",
                "809a022fe93b3d1267a2c99211400f00",
                R"({"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"WeldingDetectionReq":{"DC_EVStatus":{"EVReady":true,"EVErrorCode":"NO_ERROR","EVRESSSOC":60}}}}})"),
        TestDataContainer(
                "SessionStopReq",
                "809a022fe93b3d1267a2c991f0",
                R"({"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"SessionStopReq":{}}}})"
        )
};

// Data recorded by comemso and decoded with wireshark
// Select Payload
TestDataContainer const DIN_SPEC_TEST_DATA_COMEMSO[] = {
        TestDataContainer(
                "ContractAuthenticationReq",
                "809a0235b6479d88586be0d0b8",
                R"({"V2G_Message":{"Header":{"SessionID":"D6D91E762161AF83"},"Body":{"ContractAuthenticationReq":{}}}})"),
        TestDataContainer(
                "ContractAuthenticationRes",
                "809a0235b6479d88586be0d0c00000",
                R"({"V2G_Message":{"Header":{"SessionID":"D6D91E762161AF83"},"Body":{"ContractAuthenticationRes":{"ResponseCode":"OK","EVSEProcessing":"Finished"}}}})"),
        TestDataContainer(
                "ChargeParameterDiscoveryReq",
                "809a0235b6479d88586be0d0719008400280806010060701d0505005018243200406090a0010630a00",  // NOLINT
                R"({"V2G_Message":{"Header":{"SessionID":"D6D91E762161AF83"},"Body":{"ChargeParameterDiscoveryReq":{"EVRequestedEnergyTransferType":"DC_extended","DC_EVChargeParameter":{"DC_EVStatus":{"EVReady":false,"EVCabinConditioning":true,"EVRESSConditioning":true,"EVErrorCode":"NO_ERROR","EVRESSSOC":20},"EVMaximumCurrentLimit":{"Multiplier":1,"Unit":"A","Value":8},"EVMaximumPowerLimit":{"Multiplier":3,"Unit":"W","Value":29},"EVMaximumVoltageLimit":{"Multiplier":2,"Unit":"V","Value":5},"EVEnergyCapacity":{"Multiplier":3,"Unit":"Wh","Value":200},"EVEnergyRequest":{"Multiplier":3,"Unit":"Wh","Value":160},"FullSOC":99,"BulkSOC":80}}}}})"),
};

/* ************************************
 * Decoding
 * ********************************* */
class DecodingDIN_SPEC :public ::testing::TestWithParam<TestDataContainer> { };

TEST_P(DecodingDIN_SPEC, WithExiDecoder) {
    TestDataContainer test_data = GetParam();
    ASSERT_EQ(run_decoding(test_data, "urn:din:70121:2012:MsgDef"), test_data.json_str);
}

INSTANTIATE_TEST_CASE_P(
        ResponseMessages,
        DecodingDIN_SPEC,
        ::testing::ValuesIn(DIN_SPEC_RESPONSE_TEST_DATA));

INSTANTIATE_TEST_CASE_P(
        RequestMessages,
        DecodingDIN_SPEC,
        ::testing::ValuesIn(DIN_SPEC_REQUEST_TEST_DATA));

INSTANTIATE_TEST_CASE_P(
        DecodingComemsoDINMessages,
        DecodingDIN_SPEC,
        ::testing::ValuesIn(DIN_SPEC_TEST_DATA_COMEMSO));

/* ************************************
 * Encoding
 * ********************************* */
class EncodingDIN_SPEC :public ::testing::TestWithParam<TestDataContainer> { };

TEST_P(EncodingDIN_SPEC, WithExiDecoder) {
    TestDataContainer test_data = GetParam();
    ASSERT_EQ(run_encoding(test_data, "urn:din:70121:2012:MsgDef"), test_data.hex_str);
}

INSTANTIATE_TEST_CASE_P(
        ResponseMessages,
        EncodingDIN_SPEC,
        ::testing::ValuesIn(DIN_SPEC_RESPONSE_TEST_DATA));

INSTANTIATE_TEST_CASE_P(
        RequestMessages,
        EncodingDIN_SPEC,
        ::testing::ValuesIn(DIN_SPEC_REQUEST_TEST_DATA));

INSTANTIATE_TEST_CASE_P(
        EncodingComemsoDINMessages,
        EncodingDIN_SPEC,
        ::testing::ValuesIn(DIN_SPEC_TEST_DATA_COMEMSO));
