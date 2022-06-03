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

TEST_F(ComplexTypesTest, DecodeComplexCombination_ServiceDiscoveryRes_ChargeServiceType) {
    /* Data extracted from OpenV2G
     * ChargeServiceType --> Start
     *   FirstStartTag[START_ELEMENT(ServiceID)]
     *     getting 1bit(s) from position 116 --> 0x0000 -> 0
     *     getting 1bit(s) from position 117 --> 0x0000 -> 0
     *     getting 8bit(s) from position 118 --> 0x0001 -> 00000001
     *     getting 1bit(s) from position 126 --> 0x0000 -> 0
     *   Element[START_ELEMENT(ServiceName), START_ELEMENT(ServiceCategory)]
     *     getting 2bit(s) from position 127 --> 0x0000 -> 00
     *     getting 1bit(s) from position 129 --> 0x0000 -> 0
     *     getting 8bit(s) from position 130 --> 0x0010 -> 00010000
     *     getting 8bit(s) from position 138 --> 0x0041 -> 01000001
     *     getting 8bit(s) from position 146 --> 0x0043 -> 01000011
     *     getting 8bit(s) from position 154 --> 0x005f -> 01011111
     *     getting 8bit(s) from position 162 --> 0x0044 -> 01000100
     *     getting 8bit(s) from position 170 --> 0x0043 -> 01000011
     *     getting 8bit(s) from position 178 --> 0x005f -> 01011111
     *     getting 8bit(s) from position 186 --> 0x0043 -> 01000011
     *     getting 8bit(s) from position 194 --> 0x0068 -> 01101000
     *     getting 8bit(s) from position 202 --> 0x0061 -> 01100001
     *     getting 8bit(s) from position 210 --> 0x0072 -> 01110010
     *     getting 8bit(s) from position 218 --> 0x0067 -> 01100111
     *     getting 8bit(s) from position 226 --> 0x0069 -> 01101001
     *     getting 8bit(s) from position 234 --> 0x006e -> 01101110
     *     getting 8bit(s) from position 242 --> 0x0067 -> 01100111
     *     getting 1bit(s) from position 250 --> 0x0000 -> 0
     *   Element[START_ELEMENT(ServiceCategory)]
     *     getting 1bit(s) from position 251 --> 0x0000 -> 0
     *     getting 1bit(s) from position 252 --> 0x0000 -> 0
     *     getting 2bit(s) from position 253 --> 0x0000 -> 00
     *     getting 1bit(s) from position 255 --> 0x0000 -> 0
     *   Element[START_ELEMENT(ServiceScope), START_ELEMENT(FreeService)]
     *     getting 2bit(s) from position 256 --> 0x0001 -> 01
     *     getting 1bit(s) from position 258 --> 0x0000 -> 0
     *     getting 1bit(s) from position 259 --> 0x0000 -> 0
     *     getting 1bit(s) from position 260 --> 0x0000 -> 0
     *   Element[START_ELEMENT(SupportedEnergyTransferMode)]
     *     getting 1bit(s) from position 261 --> 0x0000 -> 0
     *     getting 1bit(s) from position 262 --> 0x0000 -> 0
     *     getting 1bit(s) from position 263 --> 0x0000 -> 0
     *     getting 3bit(s) from position 264 --> 0x0003 -> 011
     *     getting 1bit(s) from position 267 --> 0x0000 -> 0
     *     getting 2bit(s) from position 268 --> 0x0001 -> 01
     *     getting 1bit(s) from position 270 --> 0x0000 -> 0
     * ChargeServiceType --> End
     */

    std::vector<uint8_t> raw_data({0b00000000, 0b01000000, 0b01000001, 0b00000101, 0b00001101, 0b01111101,
                                   0b00010001, 0b00001101, 0b01111101, 0b00001101, 0b10100001, 0b10000101,
                                   0b11001001, 0b10011101, 0b10100101, 0b10111001, 0b10011100, 0b00000100,
                                   0b00000110, 0b01000000});
    setupWithRawData(raw_data);

    complex_types->decode_ChargeServiceType();

    string_stream->end_key();
    ASSERT_EQ(string_stream->get_full_stream(), R"({"ServiceID":1,"ServiceName":"AC_DC_Charging","ServiceCategory":"EVCharging","FreeService":false,"SupportedEnergyTransferMode":{"EnergyTransferMode":["DC_extended"]}}})");
}


TEST_F(ComplexTypesTest, DecodeComplexCombination_WeldingDetectionRes_EVSEPresentVoltage_PhysicalValueType) {
    /* Data extracted from OpenV2G
     * PhysicalValueType -> Start
     *   FirstStartTag[START_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Multiplier)]
     *     getting 1bit(s) from position 141 --> 0x0000
     *   FirstStartTag[CHARACTERS[NBIT_UNSIGNED_INTEGER]]
     *     getting 1bit(s) from position 142 --> 0x0000
     *     getting 3bit(s) from position 143 --> 0x0003
     *   END_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Multiplier)
     *     getting 1bit(s) from position 146 --> 0x0000
     *   Element[START_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Unit)]
     *     getting 1bit(s) from position 147 --> 0x0000
     *   FirstStartTag[CHARACTERS[ENUMERATION]]
     *     getting 1bit(s) from position 148 --> 0x0000
     *     getting 3bit(s) from position 149 --> 0x0004
     *   END_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Unit)
     *     getting 1bit(s) from position 152 --> 0x0000
     *   Element[START_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Value)]
     *     getting 1bit(s) from position 153 --> 0x0000
     *   First(xsi:type)StartTag[CHARACTERS[INTEGER]]
     *     getting 1bit(s) from position 154 --> 0x0000
     *     getting 1bit(s) from position 155 --> 0x0000
     *     getting 8bit(s) from position 156 --> 0x00e6
     *     getting 8bit(s) from position 164 --> 0x0001
     *   END_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Value)
     *     getting 1bit(s) from position 172 --> 0x0000
     *   Element[END_ELEMENT]
     *     getting 1bit(s) from position 173 --> 0x0000
     * PhysicalValueType -> Done
	 * -> 0b00011000, 0b10000001, 0b11001100, 0b00000010
     */

    std::vector<uint8_t> raw_data({0b00011000, 0b10000001, 0b11001100, 0b00000010, 0x00});
    setupWithRawData(raw_data);

    complex_types->decode_PhysicalValueType();

    ASSERT_EQ(string_stream->get_full_stream(), R"({"Multiplier":0,"Unit":"V","Value":230})");
}
