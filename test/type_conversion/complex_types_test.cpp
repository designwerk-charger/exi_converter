#include "base_types.h"
#include "complex_types.h"

#include "gtest/gtest.h"


class ComplexTypesTest : public ::testing::Test {
 public:
    std::shared_ptr<StringStream> string_stream;
    std::shared_ptr<BitStream> bit_stream;
    std::shared_ptr<BaseTypes> base_types;
    std::shared_ptr<EnumTypes> enum_types;
    std::shared_ptr<ComplexTypes> complex_types;

 protected:
    void SetUp() override {
        ::testing::Test::SetUp();
    }

    void setupWithRawData(std::vector<uint8_t> raw_data) {
        string_stream = std::make_shared<StringStream>("");
        bit_stream = std::make_shared<BitStream>(raw_data);
        base_types = std::make_shared<BaseTypes>(bit_stream.get());
        enum_types = std::make_shared<EnumTypes>(base_types.get());
        complex_types = std::make_shared<ComplexTypes>(base_types.get(), enum_types.get(), string_stream.get());
    }
};

TEST_F(ComplexTypesTest, DecodeElementsWithSubstitutes_PowerDeliveryResType) {
    /* Data extracted from OpenV2G
     *   DecodePowerDeliveryResType -> Start
     *   getting 1bit(s) from position 100 --> 0x0000 1
     *   getting 1bit(s) from position 101 --> 0x0000
     *   getting value for 'ResponseCode'
     *   getting 5bit(s) from position 102 --> 0x0000
     *   getting 1bit(s) from position 107 --> 0x0000
     *   getting 2bit(s) from position 108 --> 0x0001 2
     *   getting 1bit(s) from position 110 --> 0x0000
     *   getting 1bit(s) from position 111 --> 0x0000
     *   getting value for 'NotificationMaxDelay'
     *   getting 8bit(s) from position 112 --> 0x0000
     *   getting 1bit(s) from position 120 --> 0x0000
     *   getting 1bit(s) from position 121 --> 0x0000
     *   getting 1bit(s) from position 122 --> 0x0000
     *   getting value for 'EVSENotification'
     *   getting 2bit(s) from position 123 --> 0x0000
     *   getting 1bit(s) from position 125 --> 0x0000
     *   getting 2bit(s) from position 126 --> 0x0000
     *   getting 1bit(s) from position 128 --> 0x0000
     *   getting value for 'EVSEIsolationStatus'
     *   getting 3bit(s) from position 129 --> 0x0001
     *   getting 1bit(s) from position 132 --> 0x0000
     *   getting 1bit(s) from position 133 --> 0x0000
     *   getting 1bit(s) from position 134 --> 0x0000
     *   getting value for 'EVSEStatusCode'
     *   getting 4bit(s) from position 135 --> 0x0001
     *   getting 1bit(s) from position 139 --> 0x0000
     *   getting 1bit(s) from position 140 --> 0x0000
     *   getting 1bit(s) from position 141 --> 0x0000
     *   DecodePowerDeliveryResType -> Done
     */
    std::vector<uint8_t> raw_data({0b00000000, 0b01000000, 0b00000000, 0b00000001, 0b00000010, 0x00});
    setupWithRawData(raw_data);

    complex_types->decode_PowerDeliveryResType();

    ASSERT_EQ(string_stream->get_full_stream(), R"({"ResponseCode":"OK","DC_EVSEStatus":{"NotificationMaxDelay":0,"EVSENotification":"None","EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready"}})");
}

TEST_F(ComplexTypesTest, DecodeElementsList_PaymentServiceSelectionReqType_SelectedServiceList) {
    /* Data extracted from OpenV2G
     * DecodeSelectedServiceList -> Start
     *   FirstStartTag[START_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}SelectedService)]
     *     getting 1bit(s) from position 105 --> 0x0000
     *   FirstStartTag[START_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}ServiceID)]
     *     getting 1bit(s) from position 106 --> 0x0000
     *   FirstStartTag[CHARACTERS[UNSIGNED_INTEGER]]
     *     getting 1bit(s) from position 107 --> 0x0000
     *   decode ServiceID
     *     getting 8bit(s) from position 108 --> 0x0001
     *   END_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}ServiceID)
     *     getting 1bit(s) from position 116 --> 0x0000
     *   START_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}ParameterSetID), END_ELEMENT
     *     getting 2bit(s) from position 117 --> 0x0001
     *   START_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}SelectedService), END_ELEMENT
     *     getting 2bit(s) from position 119 --> 0x0001
     * DecodeSelectedServiceList -> Done
     * --> 0b00000000 00100101
     *   Element[END_ELEMENT]
     */

    std::vector<uint8_t> raw_data({0b00000000, 0b00100101, 0x00});
    setupWithRawData(raw_data);

    complex_types->decode_SelectedServiceListType();

    ASSERT_EQ(string_stream->get_full_stream(), R"({"SelectedService":[{"ServiceID":1}]})");
}

TEST_F(ComplexTypesTest, DecodeElementsList_ServiceDiscoveryRes_PaymentOptionList) {
    /* Data extracted from OpenV2G
     * PaymentOptionList --> Start
     *   PaymentOption 1
     *     getting 1bit(s) from position 109 --> 0x0000
     *     getting 1bit(s) from position 110 --> 0x0000
     *   PaymentOptionType (Enum)
     *     getting 1bit(s) from position 111 --> 0x0001
     *     getting 1bit(s) from position 112 --> 0x0000
     *   PaymentOption 2
     *     getting 2bit(s) from position 113 --> 0x0001
     * PaymentOptionList --> Done
     * -->  0b00100100
     */

    std::vector<uint8_t> raw_data({0b00100100});
    setupWithRawData(raw_data);

    complex_types->decode_PaymentOptionListType();

    ASSERT_EQ(string_stream->get_full_stream(), R"({"PaymentOption":["ExternalPayment"]})");
}

TEST_F(ComplexTypesTest, DecodeOptionalElements_ServiceDiscoveryReqType_when_empty) {
    /* Data extracted from OpenV2G
     * DecodeServiceDiscoveryReq -> Start
     *   FirstStartTag[START_ELEMENT({urn:iso:15118:2:2013:MsgBody}ServiceScope), START_ELEMENT({urn:iso:15118:2:2013:MsgBody}ServiceCategory), END_ELEMENT]
     *     getting 2bit(s) from position 100 --> 0x0002
     * DecodeServiceDiscoveryReq -> Done
     */

    std::vector<uint8_t> raw_data({0b10000000});
    setupWithRawData(raw_data);
    string_stream->start_key("ServiceDiscoveryReq");

    complex_types->decode_ServiceDiscoveryReqType();

    string_stream->end_key();
    ASSERT_EQ(string_stream->get_full_stream(), R"({"ServiceDiscoveryReq":{}})");
}
