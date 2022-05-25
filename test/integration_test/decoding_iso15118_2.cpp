#include "gtest/gtest.h"
#include "integration_test/type_conversion.h"
#include "library.h"

const TestDataContainer ISO_MSGS[] = {
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
                "8098020df6a63dcd82240f9094c8000800f0003080fa01020a1807c082014008306c1b00830781702d050000",
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
                R"({"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"SessionStopReq":{"ChargingSession":"Terminate"}}}})"),
};


std::string run_decoding(const TestDataContainer & data) {
    ExiCodec codec;

    auto input_data = hex2bin(data.hex_str);
    std::string output_data;

    try {
        output_data = codec.decode(input_data, "?");
    } catch (const std::exception& ex) {
        std::cout << "Failed decoding message " << data.name << std::endl;
    }
    std::cout << "Result:   " << output_data << std::endl;
    std::cout << "Expected: " << data.json_str << std::endl;
    return output_data;
}

TEST(DecodingISO15118_2, TestServiceDiscoveryReq) {
    uint8_t msg_num = 0;
    ASSERT_EQ(run_decoding(ISO_MSGS[msg_num]), ISO_MSGS[msg_num].json_str);
}

TEST(DecodingISO15118_2, DISABLED_TestPaymentServiceSelectionReq) {
    uint8_t msg_num = 1;
    ASSERT_EQ(run_decoding(ISO_MSGS[msg_num]), ISO_MSGS[msg_num].json_str);
}

TEST(DecodingISO15118_2, TestAuthorizationReq) {
    uint8_t msg_num = 2;
    ASSERT_EQ(run_decoding(ISO_MSGS[msg_num]), ISO_MSGS[msg_num].json_str);
}

TEST(DecodingISO15118_2, DISABLED_TestChargeParameterDiscoveryReq) {
    uint8_t msg_num = 3;
    ASSERT_EQ(run_decoding(ISO_MSGS[msg_num]), ISO_MSGS[msg_num].json_str);
}

TEST(DecodingISO15118_2, DISABLED_TestCableCheckReq) {
    uint8_t msg_num = 4;
    ASSERT_EQ(run_decoding(ISO_MSGS[msg_num]), ISO_MSGS[msg_num].json_str);
}
