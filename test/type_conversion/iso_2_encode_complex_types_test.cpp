#include "type_conversion/base_types.h"
#include "iso15118_2/complex_types.h"
#include "integration_test/type_conversion.h"

#include "gtest/gtest.h"


class Iso2EncodeComplexTypesTest : public ::testing::Test {
 public:
    std::shared_ptr<InputStringStream> string_stream;
    std::shared_ptr<BitStream> bit_stream;
    std::shared_ptr<BaseTypes> base_types;
    std::shared_ptr<iso15118_2::EnumTypes> enum_types;
    std::shared_ptr<iso15118_2::ComplexTypes> complex_types;

 protected:
    void SetUp() override {
        ::testing::Test::SetUp();
    }

    void setupWithJsonData(const std::string & input_json) {
        string_stream = std::make_shared<InputStringStream>(input_json);
        bit_stream = std::make_shared<BitStream>();
        base_types = std::make_shared<BaseTypes>(bit_stream.get(), string_stream.get());
        enum_types = std::make_shared<iso15118_2::EnumTypes>(base_types.get(), string_stream.get());
        complex_types = std::make_shared<iso15118_2::ComplexTypes>(base_types.get(),
                                                                   enum_types.get(), string_stream.get());
    }
};

inline static void compareBinaryVector(const std::vector<uint8_t> & actual, const std::vector<uint8_t> & expected) {
    std::cout << "Actual:   " << bin2hex(actual) << std::endl;
    std::cout << "Expected: " << bin2hex(expected) << std::endl;
    ASSERT_EQ(actual, expected);
}


TEST_F(Iso2EncodeComplexTypesTest, EncodeSimpleElements_PhysicalValueType) {
    /* Data extracted from OpenV2G
     *   ======= Physical Value ===========
     *   FirstStartTag[START_ELEMENT(Multiplier)]
     *   getting 1bit(s) from position 2753 --> 0x0000
     *   FirstStartTag[CHARACTERS[NBIT_UNSIGNED_INTEGER]]
     *   getting 1bit(s) from position 2754 --> 0x0000
     *   getting 3bit(s) from position 2755 --> 0x0004
     *   END_ELEMENT(Multiplier)
     *   getting 1bit(s) from position 2758 --> 0x0000
     *   Element[START_ELEMENT(Unit)]
     *   getting 1bit(s) from position 2759 --> 0x0000
     *   FirstStartTag[CHARACTERS[ENUMERATION]]
     *   getting 1bit(s) from position 2760 --> 0x0000
     *   getting 3bit(s) from position 2761 --> 0x0003
     *   END_ELEMENT(Unit)
     *   getting 1bit(s) from position 2764 --> 0x0000
     *   Element[START_ELEMENT(Value)]
     *   getting 1bit(s) from position 2765 --> 0x0000
     *   First(xsi:type)StartTag[CHARACTERS[INTEGER]]
     *   getting 1bit(s) from position 2766 --> 0x0000
     *   getting 1bit(s) from position 2767 --> 0x0000
     *   getting 8bit(s) from position 2768 --> 0x0004
     *   END_ELEMENT(Value)
     *   getting 1bit(s) from position 2776 --> 0x0000
     *   Element[END_ELEMENT]
     *   getting 1bit(s) from position 2777 --> 0x0000
     *   ======= Physical Value ===========
     */

    std::string input_json = R"({"Multiplier":1,"Unit":"A","Value":4})";
    std::vector<uint8_t> output_raw({0b00100000, 0b01100000, 0b00001000, 0x00});
    setupWithJsonData(input_json);

    complex_types->encode_PhysicalValueType();

    ASSERT_EQ(bit_stream.get()->get_exi_data(), output_raw);
}

TEST_F(Iso2EncodeComplexTypesTest, EncodeSimpleAttributes_EMAIDType) {
    // !! Really unsure if this data is correct !!
    // Data from https://github.com/SwitchEV/RISE-V2G/tree/master/RISE-V2G-Certificates/signature-creation-testdata
    std::string input_json = R"({"Id":"id4","value":"DEABCC123ABC56"})";
    std::vector<uint8_t> output_raw =
            hex2bin("02B4B21A041111505090D0CC4C8CD05090CD4D800");
    // hex input original from RiseV2G "5205696434620888A828486866264668284866A6CFA00");
    setupWithJsonData(input_json);

    complex_types->encode_EMAIDType();

    compareBinaryVector(bit_stream.get()->get_exi_data(), output_raw);
}

TEST_F(Iso2EncodeComplexTypesTest, EncodeSimpleElementList_ServiceDiscoveryRes_PaymentOptionList) {
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

    std::string input_json = R"({"PaymentOption":["ExternalPayment"]})";
    std::vector<uint8_t> output_raw = {0b00100100};
    setupWithJsonData(input_json);

    complex_types->encode_PaymentOptionListType();

    compareBinaryVector(bit_stream.get()->get_exi_data(), output_raw);
}

TEST_F(Iso2EncodeComplexTypesTest, EncodeOptionalElements_PaymentServiceSelectionReqType_SelectedServiceType) {
    /* Data extracted from OpenV2G
     * DecodeSelectedServiceList -> Start
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
     * DecodeSelectedServiceList -> Done
     */

    std::string input_json = R"({"ServiceID":1})";
    std::vector<uint8_t> output_raw = {0b0000000, 0b01001000};
    setupWithJsonData(input_json);

    complex_types->encode_SelectedServiceType();

    compareBinaryVector(bit_stream.get()->get_exi_data(), output_raw);
}

TEST_F(Iso2EncodeComplexTypesTest, EncodeComplexElementsList_PaymentServiceSelectionReqType_SelectedServiceList) {
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

    std::string input_json = R"({"SelectedService":[{"ServiceID":1}]})";
    std::vector<uint8_t> output_raw = {0b00000000, 0b00100101};
    setupWithJsonData(input_json);

    complex_types->encode_SelectedServiceListType();

    compareBinaryVector(bit_stream.get()->get_exi_data(), output_raw);
}

TEST_F(Iso2EncodeComplexTypesTest, EncodeOptionalElements_ServiceDiscoveryReqType_when_empty) {
    /* Data extracted from OpenV2G
     * DecodeServiceDiscoveryReq -> Start
     *   FirstStartTag[START_ELEMENT({urn:iso:15118:2:2013:MsgBody}ServiceScope), START_ELEMENT({urn:iso:15118:2:2013:MsgBody}ServiceCategory), END_ELEMENT]
     *     getting 2bit(s) from position 100 --> 0x0002
     * DecodeServiceDiscoveryReq -> Done
     */

    std::vector<uint8_t> raw_data({0b10000000});

    std::string input_json = R"({"ServiceDiscoveryReq":{}})";
    setupWithJsonData(input_json);

    complex_types->encode_ServiceDiscoveryReqType();

    compareBinaryVector(bit_stream.get()->get_exi_data(), raw_data);
}

TEST_F(Iso2EncodeComplexTypesTest, EncodeOptionalElements_DC_EVChargeParameterType) {
    /* Data extracted from OpenV2G
     * DC_EVChargeParameterType -> Start
     *   FirstStartTag[START_ELEMENT(DepartureTime), START_ELEMENT(DC_EVStatus)]
     *     getting 2bit(s) from position 109 --> 0x0000
     *   First(xsi:type)StartTag[CHARACTERS[UNSIGNED_INTEGER]]
     *     getting 1bit(s) from position 111 --> 0x0000
     *     getting 8bit(s) from position 112 --> 0x0000
     *   END_ELEMENT(DepartureTime)
     *     getting 1bit(s) from position 120 --> 0x0000
     *   Element[START_ELEMENT(DC_EVStatus)]
     *     getting 1bit(s) from position 121 --> 0x0000
     *   FirstStartTag[START_ELEMENT(EVReady)]
     *     getting 1bit(s) from position 122 --> 0x0000
     *     getting 1bit(s) from position 123 --> 0x0000
     *     getting 1bit(s) from position 124 --> 0x0001
     *   END_ELEMENT(EVReady)
     *     getting 1bit(s) from position 125 --> 0x0000
     *   Element[START_ELEMENT(EVErrorCode)]
     *     getting 1bit(s) from position 126 --> 0x0000
     *     getting 1bit(s) from position 127 --> 0x0000
     *     getting 4bit(s) from position 128 --> 0x0000
     *   END_ELEMENT(EVErrorCode)
     *     getting 1bit(s) from position 132 --> 0x0000
     *   Element[START_ELEMENT(EVRESSSOC)]
     *     getting 1bit(s) from position 133 --> 0x0000
     *     getting 1bit(s) from position 134 --> 0x0000
     *     getting 7bit(s) from position 135 --> 0x003c
     *   END_ELEMENT(EVRESSSOC)
     *     getting 1bit(s) from position 142 --> 0x0000
     *   Element[END_ELEMENT]
     *     getting 1bit(s) from position 143 --> 0x0000
     *   Element[START_ELEMENT(EVMaximumCurrentLimit)]
     *     getting 1bit(s) from position 144 --> 0x0000
     *   FirstStartTag[START_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Multiplier)]
     *     getting 1bit(s) from position 145 --> 0x0000
     *   FirstStartTag[CHARACTERS[NBIT_UNSIGNED_INTEGER]]
     *     getting 1bit(s) from position 146 --> 0x0000
     *     getting 3bit(s) from position 147 --> 0x0000
     *   END_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Multiplier)
     *     getting 1bit(s) from position 150 --> 0x0000
     *   Element[START_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Unit)]
     *     getting 1bit(s) from position 151 --> 0x0000
     *   FirstStartTag[CHARACTERS[ENUMERATION]]
     *     getting 1bit(s) from position 152 --> 0x0000
     *     getting 3bit(s) from position 153 --> 0x0003
     *   END_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Unit)
     *     getting 1bit(s) from position 156 --> 0x0000
     *   Element[START_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Value)]
     *     getting 1bit(s) from position 157 --> 0x0000
     *   First(xsi:type)StartTag[CHARACTERS[INTEGER]]
     *     getting 1bit(s) from position 158 --> 0x0000
     *     getting 1bit(s) from position 159 --> 0x0000
     *     getting 8bit(s) from position 160 --> 0x0080
     *     getting 8bit(s) from position 168 --> 0x00fa
     *     getting 8bit(s) from position 176 --> 0x0001
     *   END_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Value)
     *     getting 1bit(s) from position 184 --> 0x0000
     *   Element[END_ELEMENT]
     *     getting 1bit(s) from position 185 --> 0x0000
     *   Element[START_ELEMENT(EVMaximumPowerLimit), START_ELEMENT(EVMaximumVoltageLimit)]
     *     getting 2bit(s) from position 186 --> 0x0000
     *   FirstStartTag[START_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Multiplier)]
     *     getting 1bit(s) from position 188 --> 0x0000
     *   FirstStartTag[CHARACTERS[NBIT_UNSIGNED_INTEGER]]
     *     getting 1bit(s) from position 189 --> 0x0000
     *     getting 3bit(s) from position 190 --> 0x0004
     *   END_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Multiplier)
     *     getting 1bit(s) from position 193 --> 0x0000
     *   Element[START_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Unit)]
     *     getting 1bit(s) from position 194 --> 0x0000
     *   FirstStartTag[CHARACTERS[ENUMERATION]]
     *     getting 1bit(s) from position 195 --> 0x0000
     *     getting 3bit(s) from position 196 --> 0x0005
     *   END_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Unit)
     *     getting 1bit(s) from position 199 --> 0x0000
     *   Element[START_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Value)]
     *     getting 1bit(s) from position 200 --> 0x0000
     *   First(xsi:type)StartTag[CHARACTERS[INTEGER]]
     *     getting 1bit(s) from position 201 --> 0x0000
     *     getting 1bit(s) from position 202 --> 0x0000
     *     getting 8bit(s) from position 203 --> 0x00c0
     *     getting 8bit(s) from position 211 --> 0x003e
     *   END_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Value)
     *     getting 1bit(s) from position 219 --> 0x0000
     *   Element[END_ELEMENT]
     *     getting 1bit(s) from position 220 --> 0x0000
     *   Element[START_ELEMENT(EVMaximumVoltageLimit)]
     *     getting 1bit(s) from position 221 --> 0x0000
     *   FirstStartTag[START_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Multiplier)]
     *     getting 1bit(s) from position 222 --> 0x0000
     *   FirstStartTag[CHARACTERS[NBIT_UNSIGNED_INTEGER]]
     *     getting 1bit(s) from position 223 --> 0x0000
     *     getting 3bit(s) from position 224 --> 0x0004
     *   END_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Multiplier)
     *     getting 1bit(s) from position 227 --> 0x0000
     *   Element[START_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Unit)]
     *     getting 1bit(s) from position 228 --> 0x0000
     *   FirstStartTag[CHARACTERS[ENUMERATION]]
     *     getting 1bit(s) from position 229 --> 0x0000
     *     getting 3bit(s) from position 230 --> 0x0004
     *   END_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Unit)
     *     getting 1bit(s) from position 233 --> 0x0000
     *   Element[START_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Value)]
     *     getting 1bit(s) from position 234 --> 0x0000
     *   First(xsi:type)StartTag[CHARACTERS[INTEGER]]
     *     getting 1bit(s) from position 235 --> 0x0000
     *     getting 1bit(s) from position 236 --> 0x0000
     *     getting 8bit(s) from position 237 --> 0x0028
     *   END_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Value)
     *     getting 1bit(s) from position 245 --> 0x0000
     *   Element[END_ELEMENT]
     *     getting 1bit(s) from position 246 --> 0x0000
     *   Element[START_ELEMENT(EVEnergyCapacity), START_ELEMENT(EVEnergyRequest), START_ELEMENT(FullSOC), START_ELEMENT(BulkSOC), END_ELEMENT]
     *     getting 3bit(s) from position 247 --> 0x0000
     *   FirstStartTag[START_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Multiplier)]
     *     getting 1bit(s) from position 250 --> 0x0000
     *   FirstStartTag[CHARACTERS[NBIT_UNSIGNED_INTEGER]]
     *     getting 1bit(s) from position 251 --> 0x0000
     *     getting 3bit(s) from position 252 --> 0x0004
     *   END_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Multiplier)
     *     getting 1bit(s) from position 255 --> 0x0000
     *   Element[START_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Unit)]
     *     getting 1bit(s) from position 256 --> 0x0000
     *   FirstStartTag[CHARACTERS[ENUMERATION]]
     *     getting 1bit(s) from position 257 --> 0x0000
     *     getting 3bit(s) from position 258 --> 0x0006
     *   END_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Unit)
     *     getting 1bit(s) from position 261 --> 0x0000
     *   Element[START_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Value)]
     *     getting 1bit(s) from position 262 --> 0x0000
     *   First(xsi:type)StartTag[CHARACTERS[INTEGER]]
     *     getting 1bit(s) from position 263 --> 0x0000
     *     getting 1bit(s) from position 264 --> 0x0000
     *     getting 8bit(s) from position 265 --> 0x00d8
     *     getting 8bit(s) from position 273 --> 0x0036
     *   END_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Value)
     *     getting 1bit(s) from position 281 --> 0x0000
     *   Element[END_ELEMENT]
     *     getting 1bit(s) from position 282 --> 0x0000
     *   Element[START_ELEMENT(EVEnergyRequest), START_ELEMENT(FullSOC), START_ELEMENT(BulkSOC), END_ELEMENT]
     *     getting 3bit(s) from position 283 --> 0x0000
     *   FirstStartTag[START_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Multiplier)]
     *     getting 1bit(s) from position 286 --> 0x0000
     *   FirstStartTag[CHARACTERS[NBIT_UNSIGNED_INTEGER]]
     *     getting 1bit(s) from position 287 --> 0x0000
     *     getting 3bit(s) from position 288 --> 0x0004
     *   END_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Multiplier)
     *     getting 1bit(s) from position 291 --> 0x0000
     *   Element[START_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Unit)]
     *     getting 1bit(s) from position 292 --> 0x0000
     *   FirstStartTag[CHARACTERS[ENUMERATION]]
     *     getting 1bit(s) from position 293 --> 0x0000
     *     getting 3bit(s) from position 294 --> 0x0006
     *   END_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Unit)
     *     getting 1bit(s) from position 297 --> 0x0000
     *   Element[START_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Value)]
     *     getting 1bit(s) from position 298 --> 0x0000
     *   First(xsi:type)StartTag[CHARACTERS[INTEGER]]
     *     getting 1bit(s) from position 299 --> 0x0000
     *     getting 1bit(s) from position 300 --> 0x0000
     *     getting 8bit(s) from position 301 --> 0x00f0
     *     getting 8bit(s) from position 309 --> 0x002e
     *   END_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Value)
     *     getting 1bit(s) from position 317 --> 0x0000
     *   Element[END_ELEMENT]
     *     getting 1bit(s) from position 318 --> 0x0000
     *   Element[START_ELEMENT(FullSOC), START_ELEMENT(BulkSOC), END_ELEMENT]
     *     getting 2bit(s) from position 319 --> 0x0000
     *     getting 1bit(s) from position 321 --> 0x0000
     *     getting 7bit(s) from position 322 --> 0x005a
     *   END_ELEMENT(FullSOC)
     *     getting 1bit(s) from position 329 --> 0x0000
     *   Element[START_ELEMENT(BulkSOC), END_ELEMENT]
     *     getting 2bit(s) from position 330 --> 0x0000
     *     getting 1bit(s) from position 332 --> 0x0000
     *     getting 7bit(s) from position 333 --> 0x0050
     *   END_ELEMENT(BulkSOC)
     *     getting 1bit(s) from position 340 --> 0x0000
     *   Element[END_ELEMENT]
     *     getting 1bit(s) from position 341 --> 0x0000
     * DC_EVChargeParameterType -> Done
     */

    std::vector<uint8_t> raw_data({0b00000000, 0b00000001, 0b00000000, 0b00011110, 0b00000000, 0b00000110,
                                   0b00010000, 0b00011111, 0b01000000, 0b00100000, 0b01000001, 0b01000011,
                                   0b00000000, 0b11111000, 0b00010000, 0b01000000, 0b00101000, 0b00000001,
                                   0b00000110, 0b00001101, 0b10000011, 0b01100000, 0b00010000, 0b01100000,
                                   0b11110000, 0b00101110, 0b00000101, 0b10100000, 0b10100000});

    std::string input_json = R"({"DepartureTime":0,"DC_EVStatus":{"EVReady":true,"EVErrorCode":"NO_ERROR","EVRESSSOC":60},"EVMaximumCurrentLimit":{"Multiplier":-3,"Unit":"A","Value":32000},"EVMaximumPowerLimit":{"Multiplier":1,"Unit":"W","Value":8000},"EVMaximumVoltageLimit":{"Multiplier":1,"Unit":"V","Value":40},"EVEnergyCapacity":{"Multiplier":1,"Unit":"Wh","Value":7000},"EVEnergyRequest":{"Multiplier":1,"Unit":"Wh","Value":6000},"FullSOC":90,"BulkSOC":80})";
    setupWithJsonData(input_json);

    complex_types->encode_DC_EVChargeParameterType();

    compareBinaryVector(bit_stream.get()->get_exi_data(), raw_data);
}

