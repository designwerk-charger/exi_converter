#include "type_conversion/base_types.h"
#include "iso15118_2/complex_types.h"
#include "integration_test/type_conversion.h"

#include "gtest/gtest.h"


class Iso2EncodeComplexTypesTest : public ::testing::Test {
 public:
    std::shared_ptr<JObject> json_object;
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
        json_object = JsonParser::parse(input_json);
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

    complex_types->encode_PhysicalValueType(json_object);

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

    complex_types->encode_EMAIDType(json_object);

    compareBinaryVector(bit_stream.get()->get_exi_data(), output_raw);
}

TEST_F(Iso2EncodeComplexTypesTest, EncodeElementsWithSubstitutes_PowerDeliveryResType) {
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

    std::string input_json = R"({"ResponseCode":"OK","DC_EVSEStatus":{"NotificationMaxDelay":0,"EVSENotification":"None","EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready"}})";
    std::vector<uint8_t> output_raw({0b00000000, 0b01000000, 0b00000000, 0b00000001, 0b00000010, 0x00});
    setupWithJsonData(input_json);

    complex_types->encode_PowerDeliveryResType(json_object);

    compareBinaryVector(bit_stream.get()->get_exi_data(), output_raw);
}

TEST_F(Iso2EncodeComplexTypesTest, EncodeSimpleElementList_ServiceDiscoveryRes_PaymentOptionList_withOneItem) {
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
    std::vector<uint8_t> output_raw = {0x24};
    setupWithJsonData(input_json);

    complex_types->encode_PaymentOptionListType(json_object);

    compareBinaryVector(bit_stream.get()->get_exi_data(), output_raw);
}

TEST_F(Iso2EncodeComplexTypesTest, EncodeSimpleElementList_ServiceDiscoveryRes_PaymentOptionList_withTwoItem) {
    std::string input_json = R"({"PaymentOption":["Contract","ExternalPayment"]})";
    std::vector<uint8_t> output_raw = {0x01, 0x00};
    setupWithJsonData(input_json);

    complex_types->encode_PaymentOptionListType(json_object);

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

    complex_types->encode_SelectedServiceType(json_object);

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

    complex_types->encode_SelectedServiceListType(json_object);

    compareBinaryVector(bit_stream.get()->get_exi_data(), output_raw);
}

TEST_F(Iso2EncodeComplexTypesTest, EncodeOptionalElements_ServiceDiscoveryReqType_when_empty) {
    /* INPUT:  {"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"ServiceDiscoveryReq":{}}}}
       EXI:    8098020df6a63dcd82240f91b8 */
    /* Data extracted from OpenV2G
     * DecodeServiceDiscoveryReq -> Start
     *   FirstStartTag[START_ELEMENT({urn:iso:15118:2:2013:MsgBody}ServiceScope), START_ELEMENT({urn:iso:15118:2:2013:MsgBody}ServiceCategory), END_ELEMENT]
     *     getting 2bit(s) from position 100 --> 0x0002
     * DecodeServiceDiscoveryReq -> Done
     */

    std::vector<uint8_t> raw_data({0b10000000});

    std::string input_json = R"({"ServiceDiscoveryReq":{}})";
    setupWithJsonData(input_json);

    complex_types->encode_ServiceDiscoveryReqType(json_object->get_object("ServiceDiscoveryReq"));

    compareBinaryVector(bit_stream.get()->get_exi_data(), raw_data);
}

TEST_F(Iso2EncodeComplexTypesTest, EncodeOptionalElements_ServiceDiscoveryReqType_when_ServiceScopeOnly) {
    /* INPUT:  {"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"ServiceDiscoveryReq":{"ServiceScope":"bla"}}}}
       EXI:    8098020df6a63dcd82240f91b00ac4d8c240 */
    std::vector<uint8_t> raw_data({0x00, 0xac, 0x4d, 0x8c, 0x24});

    std::string input_json = R"({"ServiceScope":"bla"})";
    setupWithJsonData(input_json);

    complex_types->encode_ServiceDiscoveryReqType(json_object);

    compareBinaryVector(bit_stream.get()->get_exi_data(), raw_data);
}

TEST_F(Iso2EncodeComplexTypesTest, EncodeOptionalElements_ServiceDiscoveryReqType_when_ServiceCategoryOnly) {
    /* INPUT:  {"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"ServiceDiscoveryReq":{"ServiceCategory":"Internet"}}}}
       EXI:    8098020df6a63dcd82240f91b480 */
    std::vector<uint8_t> raw_data({0x48});

    std::string input_json = R"({"ServiceCategory":"Internet"})";
    setupWithJsonData(input_json);

    complex_types->encode_ServiceDiscoveryReqType(json_object);

    compareBinaryVector(bit_stream.get()->get_exi_data(), raw_data);
}

TEST_F(Iso2EncodeComplexTypesTest, EncodeOptionalElements_ServiceDiscoveryReqType_when_ServiceScopeAndServiceCategory) {
    /* INPUT:  {"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"ServiceDiscoveryReq":{"ServiceScope":"bla","ServiceCategory":"Internet"}}}}
       EXI:    8098020df6a63dcd82240f91b00ac4d8c20800 */
    std::vector<uint8_t> raw_data({0x00, 0xac, 0x4d, 0x8c, 0x20, 0x80});

    std::string input_json = R"({"ServiceScope":"bla","ServiceCategory":"Internet"})";
    setupWithJsonData(input_json);

    complex_types->encode_ServiceDiscoveryReqType(json_object);

    compareBinaryVector(bit_stream.get()->get_exi_data(), raw_data);
}

TEST_F(Iso2EncodeComplexTypesTest, EncodeOptionalElements_DC_EVChargeParameterType__when_all_options_available) {
    /* INPUT:  {"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"ChargeParameterDiscoveryReq":{"RequestedEnergyTransferMode":"DC_extended","DC_EVChargeParameter":{"DepartureTime":0,"DC_EVStatus":{"EVReady":true,"EVErrorCode":"NO_ERROR","EVRESSSOC":60},"EVMaximumCurrentLimit":{"Multiplier":-3,"Unit":"A","Value":32000},"EVMaximumPowerLimit":{"Multiplier":1,"Unit":"W","Value":8000},"EVMaximumVoltageLimit":{"Multiplier":1,"Unit":"V","Value":40},"EVEnergyCapacity":{"Multiplier":1,"Unit":"Wh","Value":7000},"EVEnergyRequest":{"Multiplier":1,"Unit":"Wh","Value":6000},"FullSOC":90,"BulkSOC":80}}}}}
       EXI:    8098020df6a63dcd82240f9094c8000800f0003080fa01020a1807c082014008306c1b00830781702d050000 */

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

    complex_types->encode_DC_EVChargeParameterType(json_object);

    compareBinaryVector(bit_stream->get_exi_data(), raw_data);
}

TEST_F(Iso2EncodeComplexTypesTest, EncodeOptionalElements_DC_EVChargeParameterType__when_no_options_available) {
    /* INPUT:  {"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"ChargeParameterDiscoveryReq":{"RequestedEnergyTransferMode":"DC_extended","DC_EVChargeParameter":{"DC_EVStatus":{"EVReady":true,"EVErrorCode":"NO_ERROR","EVRESSSOC":60},"EVMaximumCurrentLimit":{"Multiplier":-3,"Unit":"A","Value":32000},"EVMaximumVoltageLimit":{"Multiplier":1,"Unit":"V","Value":40}}}}}}
       EXI:    8098020df6a63dcd82240f9094ca400780018407d00890402820 */

    std::vector<uint8_t> raw_data({0x48, 0x00, 0xf0, 0x00, 0x30, 0x80, 0xfa, 0x01, 0x12, 0x08, 0x05, 0x04});

    std::string input_json = R"({"DC_EVStatus":{"EVReady":true,"EVErrorCode":"NO_ERROR","EVRESSSOC":60},"EVMaximumCurrentLimit":{"Multiplier":-3,"Unit":"A","Value":32000},"EVMaximumVoltageLimit":{"Multiplier":1,"Unit":"V","Value":40}})";
    setupWithJsonData(input_json);

    complex_types->encode_DC_EVChargeParameterType(json_object);

    compareBinaryVector(bit_stream->get_exi_data(), raw_data);
}

TEST_F(Iso2EncodeComplexTypesTest, EncodeOptionalElements_MessageHeaderTypeWithoutOptionalParts) {
    /* Start iso header
    FirstStartTag[START_ELEMENT(SessionID)]
        getting 1bit(s) from position 16 --> 0x0000
        getting 1bit(s) from position 17 --> 0x0000
        getting 8bit(s) from position 18 --> 0x0008
        getting 8bit(s) from position 26 --> 0x0037
        getting 8bit(s) from position 34 --> 0x00da
        getting 8bit(s) from position 42 --> 0x0098
        getting 8bit(s) from position 50 --> 0x00f7
        getting 8bit(s) from position 58 --> 0x0036
        getting 8bit(s) from position 66 --> 0x0008
        getting 8bit(s) from position 74 --> 0x0090
        getting 8bit(s) from position 82 --> 0x003e
    END_ELEMENT(SessionID)
        getting 1bit(s) from position 90 --> 0x0000
    Element[START_ELEMENT(Notification), START_ELEMENT(Signature), END_ELEMENT]
        getting 2bit(s) from position 91 --> 0x0002
    End iso header
     */

    std::vector<uint8_t> raw_data({0b00000010, 0b00001101, 0b11110110,
                                   0b10100110, 0b00111101, 0b11001101, 0b10000010,
                                   0b00100100, 0b00001111, 0b10010000});

    std::string input_json = R"({"SessionID":"37DA98F73608903E"})";
    setupWithJsonData(input_json);

    complex_types->encode_MessageHeaderType(json_object);

    compareBinaryVector(bit_stream->get_exi_data(), raw_data);
}

TEST_F(Iso2EncodeComplexTypesTest, EncodeComplexCombination_MessageHeaderTypeWithOptionalParts) {
    /* Data extracted from OpenV2G
     * Start iso header
     *   FirstStartTag[START_ELEMENT(SessionID)]
     *     getting 1bit(s) from position 16 --> 0x0000
     *     getting 1bit(s) from position 17 --> 0x0000
     *     getting 8bit(s) from position 18 --> 0x0008
     *     getting 8bit(s) from position 26 --> 0x0037
     *     getting 8bit(s) from position 34 --> 0x00da
     *     getting 8bit(s) from position 42 --> 0x0098
     *     getting 8bit(s) from position 50 --> 0x00f7
     *     getting 8bit(s) from position 58 --> 0x0036
     *     getting 8bit(s) from position 66 --> 0x0008
     *     getting 8bit(s) from position 74 --> 0x0090
     *     getting 8bit(s) from position 82 --> 0x003e
     *     getting 1bit(s) from position 90 --> 0x0000
     *   Element[START_ELEMENT(Notification), START_ELEMENT(Signature), END_ELEMENT]
     *     getting 2bit(s) from position 91 --> 0x0001
     * SignatureType -> Start
     *   FirstStartTag[ATTRIBUTE[STRING](Id), START_ELEMENT(SignedInfo)]
     *     getting 2bit(s) from position 93 --> 0x0001
     * SignedInfo -> Start
     *   FirstStartTag[ATTRIBUTE[STRING](Id), START_ELEMENT(CanonicalizationMethod)]
     *     getting 2bit(s) from position 95 --> 0x0001
     *     getting 1bit(s) from position 97 --> 0x0000
     *     getting 8bit(s) from position 98 --> 0x0025
     *     getting 8bit(s) from position 106 --> 0x0068
     *     getting 8bit(s) from position 114 --> 0x0074
     *     getting 8bit(s) from position 122 --> 0x0074
     *     getting 8bit(s) from position 130 --> 0x0070
     *     getting 8bit(s) from position 138 --> 0x003a
     *     getting 8bit(s) from position 146 --> 0x002f
     *     getting 8bit(s) from position 154 --> 0x002f
     *     getting 8bit(s) from position 162 --> 0x0077
     *     getting 8bit(s) from position 170 --> 0x0077
     *     getting 8bit(s) from position 178 --> 0x0077
     *     getting 8bit(s) from position 186 --> 0x002e
     *     getting 8bit(s) from position 194 --> 0x0077
     *     getting 8bit(s) from position 202 --> 0x0033
     *     getting 8bit(s) from position 210 --> 0x002e
     *     getting 8bit(s) from position 218 --> 0x006f
     *     getting 8bit(s) from position 226 --> 0x0072
     *     getting 8bit(s) from position 234 --> 0x0067
     *     getting 8bit(s) from position 242 --> 0x002f
     *     getting 8bit(s) from position 250 --> 0x0054
     *     getting 8bit(s) from position 258 --> 0x0052
     *     getting 8bit(s) from position 266 --> 0x002f
     *     getting 8bit(s) from position 274 --> 0x0063
     *     getting 8bit(s) from position 282 --> 0x0061
     *     getting 8bit(s) from position 290 --> 0x006e
     *     getting 8bit(s) from position 298 --> 0x006f
     *     getting 8bit(s) from position 306 --> 0x006e
     *     getting 8bit(s) from position 314 --> 0x0069
     *     getting 8bit(s) from position 322 --> 0x0063
     *     getting 8bit(s) from position 330 --> 0x0061
     *     getting 8bit(s) from position 338 --> 0x006c
     *     getting 8bit(s) from position 346 --> 0x002d
     *     getting 8bit(s) from position 354 --> 0x0065
     *     getting 8bit(s) from position 362 --> 0x0078
     *     getting 8bit(s) from position 370 --> 0x0069
     *     getting 8bit(s) from position 378 --> 0x002f
     *     getting 2bit(s) from position 386 --> 0x0001
     *   Element[START_ELEMENT(SignatureMethod)]
     *     getting 1bit(s) from position 388 --> 0x0000
     *     getting 1bit(s) from position 389 --> 0x0000
     *     getting 8bit(s) from position 390 --> 0x0035
     *     getting 8bit(s) from position 398 --> 0x0068
     *     getting 8bit(s) from position 406 --> 0x0074
     *     getting 8bit(s) from position 414 --> 0x0074
     *     getting 8bit(s) from position 422 --> 0x0070
     *     getting 8bit(s) from position 430 --> 0x003a
     *     getting 8bit(s) from position 438 --> 0x002f
     *     getting 8bit(s) from position 446 --> 0x002f
     *     getting 8bit(s) from position 454 --> 0x0077
     *     getting 8bit(s) from position 462 --> 0x0077
     *     getting 8bit(s) from position 470 --> 0x0077
     *     getting 8bit(s) from position 478 --> 0x002e
     *     getting 8bit(s) from position 486 --> 0x0077
     *     getting 8bit(s) from position 494 --> 0x0033
     *     getting 8bit(s) from position 502 --> 0x002e
     *     getting 8bit(s) from position 510 --> 0x006f
     *     getting 8bit(s) from position 518 --> 0x0072
     *     getting 8bit(s) from position 526 --> 0x0067
     *     getting 8bit(s) from position 534 --> 0x002f
     *     getting 8bit(s) from position 542 --> 0x0032
     *     getting 8bit(s) from position 550 --> 0x0030
     *     getting 8bit(s) from position 558 --> 0x0030
     *     getting 8bit(s) from position 566 --> 0x0031
     *     getting 8bit(s) from position 574 --> 0x002f
     *     getting 8bit(s) from position 582 --> 0x0030
     *     getting 8bit(s) from position 590 --> 0x0034
     *     getting 8bit(s) from position 598 --> 0x002f
     *     getting 8bit(s) from position 606 --> 0x0078
     *     getting 8bit(s) from position 614 --> 0x006d
     *     getting 8bit(s) from position 622 --> 0x006c
     *     getting 8bit(s) from position 630 --> 0x0064
     *     getting 8bit(s) from position 638 --> 0x0073
     *     getting 8bit(s) from position 646 --> 0x0069
     *     getting 8bit(s) from position 654 --> 0x0067
     *     getting 8bit(s) from position 662 --> 0x002d
     *     getting 8bit(s) from position 670 --> 0x006d
     *     getting 8bit(s) from position 678 --> 0x006f
     *     getting 8bit(s) from position 686 --> 0x0072
     *     getting 8bit(s) from position 694 --> 0x0065
     *     getting 8bit(s) from position 702 --> 0x0023
     *     getting 8bit(s) from position 710 --> 0x0065
     *     getting 8bit(s) from position 718 --> 0x0063
     *     getting 8bit(s) from position 726 --> 0x0064
     *     getting 8bit(s) from position 734 --> 0x0073
     *     getting 8bit(s) from position 742 --> 0x0061
     *     getting 8bit(s) from position 750 --> 0x002d
     *     getting 8bit(s) from position 758 --> 0x0073
     *     getting 8bit(s) from position 766 --> 0x0068
     *     getting 8bit(s) from position 774 --> 0x0061
     *     getting 8bit(s) from position 782 --> 0x0032
     *     getting 8bit(s) from position 790 --> 0x0035
     *     getting 8bit(s) from position 798 --> 0x0036
     *     getting 3bit(s) from position 806 --> 0x0002
     *   Element[START_ELEMENT(Reference)]
     *     getting 1bit(s) from position 809 --> 0x0000
     *   FirstStartTag[ATTRIBUTE[STRING](Id), ATTRIBUTE[STRING](Type), ATTRIBUTE[STRING](URI), START_ELEMENT(Transforms), START_ELEMENT(DigestMethod)]
     *     getting 3bit(s) from position 810 --> 0x0002
     *     getting 8bit(s) from position 813 --> 0x0006
     *     getting 8bit(s) from position 821 --> 0x0023
     *     getting 8bit(s) from position 829 --> 0x0069
     *     getting 8bit(s) from position 837 --> 0x0064
     *     getting 8bit(s) from position 845 --> 0x0031
     *   StartTag[START_ELEMENT(Transforms), START_ELEMENT(DigestMethod)]
     *     getting 2bit(s) from position 853 --> 0x0000
     *     getting 1bit(s) from position 855 --> 0x0000
     *     getting 1bit(s) from position 856 --> 0x0000
     *     getting 8bit(s) from position 857 --> 0x0025
     *     getting 8bit(s) from position 865 --> 0x0068
     *     getting 8bit(s) from position 873 --> 0x0074
     *     getting 8bit(s) from position 881 --> 0x0074
     *     getting 8bit(s) from position 889 --> 0x0070
     *     getting 8bit(s) from position 897 --> 0x003a
     *     getting 8bit(s) from position 905 --> 0x002f
     *     getting 8bit(s) from position 913 --> 0x002f
     *     getting 8bit(s) from position 921 --> 0x0077
     *     getting 8bit(s) from position 929 --> 0x0077
     *     getting 8bit(s) from position 937 --> 0x0077
     *     getting 8bit(s) from position 945 --> 0x002e
     *     getting 8bit(s) from position 953 --> 0x0077
     *     getting 8bit(s) from position 961 --> 0x0033
     *     getting 8bit(s) from position 969 --> 0x002e
     *     getting 8bit(s) from position 977 --> 0x006f
     *     getting 8bit(s) from position 985 --> 0x0072
     *     getting 8bit(s) from position 993 --> 0x0067
     *     getting 8bit(s) from position 1001 --> 0x002f
     *     getting 8bit(s) from position 1009 --> 0x0054
     *     getting 8bit(s) from position 1017 --> 0x0052
     *     getting 8bit(s) from position 1025 --> 0x002f
     *     getting 8bit(s) from position 1033 --> 0x0063
     *     getting 8bit(s) from position 1041 --> 0x0061
     *     getting 8bit(s) from position 1049 --> 0x006e
     *     getting 8bit(s) from position 1057 --> 0x006f
     *     getting 8bit(s) from position 1065 --> 0x006e
     *     getting 8bit(s) from position 1073 --> 0x0069
     *     getting 8bit(s) from position 1081 --> 0x0063
     *     getting 8bit(s) from position 1089 --> 0x0061
     *     getting 8bit(s) from position 1097 --> 0x006c
     *     getting 8bit(s) from position 1105 --> 0x002d
     *     getting 8bit(s) from position 1113 --> 0x0065
     *     getting 8bit(s) from position 1121 --> 0x0078
     *     getting 8bit(s) from position 1129 --> 0x0069
     *     getting 8bit(s) from position 1137 --> 0x002f
     *     getting 3bit(s) from position 1145 --> 0x0002
     *     getting 2bit(s) from position 1148 --> 0x0001
     *   Element[START_ELEMENT(DigestMethod)]
     *     getting 1bit(s) from position 1150 --> 0x0000
     *     getting 1bit(s) from position 1151 --> 0x0000
     *     getting 8bit(s) from position 1152 --> 0x0029
     *     getting 8bit(s) from position 1160 --> 0x0068
     *     getting 8bit(s) from position 1168 --> 0x0074
     *     getting 8bit(s) from position 1176 --> 0x0074
     *     getting 8bit(s) from position 1184 --> 0x0070
     *     getting 8bit(s) from position 1192 --> 0x003a
     *     getting 8bit(s) from position 1200 --> 0x002f
     *     getting 8bit(s) from position 1208 --> 0x002f
     *     getting 8bit(s) from position 1216 --> 0x0077
     *     getting 8bit(s) from position 1224 --> 0x0077
     *     getting 8bit(s) from position 1232 --> 0x0077
     *     getting 8bit(s) from position 1240 --> 0x002e
     *     getting 8bit(s) from position 1248 --> 0x0077
     *     getting 8bit(s) from position 1256 --> 0x0033
     *     getting 8bit(s) from position 1264 --> 0x002e
     *     getting 8bit(s) from position 1272 --> 0x006f
     *     getting 8bit(s) from position 1280 --> 0x0072
     *     getting 8bit(s) from position 1288 --> 0x0067
     *     getting 8bit(s) from position 1296 --> 0x002f
     *     getting 8bit(s) from position 1304 --> 0x0032
     *     getting 8bit(s) from position 1312 --> 0x0030
     *     getting 8bit(s) from position 1320 --> 0x0030
     *     getting 8bit(s) from position 1328 --> 0x0031
     *     getting 8bit(s) from position 1336 --> 0x002f
     *     getting 8bit(s) from position 1344 --> 0x0030
     *     getting 8bit(s) from position 1352 --> 0x0034
     *     getting 8bit(s) from position 1360 --> 0x002f
     *     getting 8bit(s) from position 1368 --> 0x0078
     *     getting 8bit(s) from position 1376 --> 0x006d
     *     getting 8bit(s) from position 1384 --> 0x006c
     *     getting 8bit(s) from position 1392 --> 0x0065
     *     getting 8bit(s) from position 1400 --> 0x006e
     *     getting 8bit(s) from position 1408 --> 0x0063
     *     getting 8bit(s) from position 1416 --> 0x0023
     *     getting 8bit(s) from position 1424 --> 0x0073
     *     getting 8bit(s) from position 1432 --> 0x0068
     *     getting 8bit(s) from position 1440 --> 0x0061
     *     getting 8bit(s) from position 1448 --> 0x0032
     *     getting 8bit(s) from position 1456 --> 0x0035
     *     getting 8bit(s) from position 1464 --> 0x0036
     *     getting 2bit(s) from position 1472 --> 0x0001
     *   Element[START_ELEMENT(DigestValue)]
     *     getting 1bit(s) from position 1474 --> 0x0000
     *     getting 1bit(s) from position 1475 --> 0x0000
     *     getting 8bit(s) from position 1476 --> 0x0020
     *     getting 8bit(s) from position 1484 --> 0x0014
     *     getting 8bit(s) from position 1492 --> 0x00e3
     *     getting 8bit(s) from position 1500 --> 0x00d8
     *     getting 8bit(s) from position 1508 --> 0x007a
     *     getting 8bit(s) from position 1516 --> 0x00e2
     *     getting 8bit(s) from position 1524 --> 0x0042
     *     getting 8bit(s) from position 1532 --> 0x004b
     *     getting 8bit(s) from position 1540 --> 0x006c
     *     getting 8bit(s) from position 1548 --> 0x0030
     *     getting 8bit(s) from position 1556 --> 0x005f
     *     getting 8bit(s) from position 1564 --> 0x00d5
     *     getting 8bit(s) from position 1572 --> 0x00f1
     *     getting 8bit(s) from position 1580 --> 0x008a
     *     getting 8bit(s) from position 1588 --> 0x0024
     *     getting 8bit(s) from position 1596 --> 0x00fe
     *     getting 8bit(s) from position 1604 --> 0x002f
     *     getting 8bit(s) from position 1612 --> 0x002f
     *     getting 8bit(s) from position 1620 --> 0x0068
     *     getting 8bit(s) from position 1628 --> 0x007c
     *     getting 8bit(s) from position 1636 --> 0x00d6
     *     getting 8bit(s) from position 1644 --> 0x0058
     *     getting 8bit(s) from position 1652 --> 0x00d5
     *     getting 8bit(s) from position 1660 --> 0x009b
     *     getting 8bit(s) from position 1668 --> 0x00be
     *     getting 8bit(s) from position 1676 --> 0x0051
     *     getting 8bit(s) from position 1684 --> 0x00a7
     *     getting 8bit(s) from position 1692 --> 0x00ce
     *     getting 8bit(s) from position 1700 --> 0x003f
     *     getting 8bit(s) from position 1708 --> 0x005c
     *     getting 8bit(s) from position 1716 --> 0x00f0
     *     getting 8bit(s) from position 1724 --> 0x0086
     *     getting 8bit(s) from position 1732 --> 0x00f0
     *     getting 1bit(s) from position 1740 --> 0x0000
     *   Element[END_ELEMENT]
     *     getting 1bit(s) from position 1741 --> 0x0000
     *   Element[START_ELEMENT(Reference), END_ELEMENT]
     *     getting 2bit(s) from position 1742 --> 0x0001
     * SignedInfo -> Done
     *   Element[START_ELEMENT(SignatureValue)]
     *     getting 1bit(s) from position 1744 --> 0x0000
     * signatureValue -> Start
     *   FirstStartTag[ATTRIBUTE[STRING](Id), CHARACTERS[BINARY_BASE64]]
     *     getting 2bit(s) from position 1745 --> 0x0001
     *   CHARACTERS[BINARY_BASE64]
     *     getting 8bit(s) from position 1747 --> 0x0046
     *     getting 8bit(s) from position 1755 --> 0x0030
     *     getting 8bit(s) from position 1763 --> 0x0044
     *     getting 8bit(s) from position 1771 --> 0x0002
     *     getting 8bit(s) from position 1779 --> 0x0020
     *     getting 8bit(s) from position 1787 --> 0x0056
     *     getting 8bit(s) from position 1795 --> 0x00a2
     *     getting 8bit(s) from position 1803 --> 0x0080
     *     getting 8bit(s) from position 1811 --> 0x009f
     *     getting 8bit(s) from position 1819 --> 0x003d
     *     getting 8bit(s) from position 1827 --> 0x0052
     *     getting 8bit(s) from position 1835 --> 0x003a
     *     getting 8bit(s) from position 1843 --> 0x0025
     *     getting 8bit(s) from position 1851 --> 0x0037
     *     getting 8bit(s) from position 1859 --> 0x0032
     *     getting 8bit(s) from position 1867 --> 0x0029
     *     getting 8bit(s) from position 1875 --> 0x00e5
     *     getting 8bit(s) from position 1883 --> 0x006c
     *     getting 8bit(s) from position 1891 --> 0x00f5
     *     getting 8bit(s) from position 1899 --> 0x00b5
     *     getting 8bit(s) from position 1907 --> 0x004b
     *     getting 8bit(s) from position 1915 --> 0x0076
     *     getting 8bit(s) from position 1923 --> 0x008d
     *     getting 8bit(s) from position 1931 --> 0x0083
     *     getting 8bit(s) from position 1939 --> 0x00f2
     *     getting 8bit(s) from position 1947 --> 0x0021
     *     getting 8bit(s) from position 1955 --> 0x0043
     *     getting 8bit(s) from position 1963 --> 0x005e
     *     getting 8bit(s) from position 1971 --> 0x0070
     *     getting 8bit(s) from position 1979 --> 0x00d7
     *     getting 8bit(s) from position 1987 --> 0x0026
     *     getting 8bit(s) from position 1995 --> 0x0074
     *     getting 8bit(s) from position 2003 --> 0x00cf
     *     getting 8bit(s) from position 2011 --> 0x00c5
     *     getting 8bit(s) from position 2019 --> 0x0047
     *     getting 8bit(s) from position 2027 --> 0x0028
     *     getting 8bit(s) from position 2035 --> 0x0099
     *     getting 8bit(s) from position 2043 --> 0x0002
     *     getting 8bit(s) from position 2051 --> 0x0020
     *     getting 8bit(s) from position 2059 --> 0x001c
     *     getting 8bit(s) from position 2067 --> 0x0079
     *     getting 8bit(s) from position 2075 --> 0x000c
     *     getting 8bit(s) from position 2083 --> 0x00dd
     *     getting 8bit(s) from position 2091 --> 0x00f6
     *     getting 8bit(s) from position 2099 --> 0x008f
     *     getting 8bit(s) from position 2107 --> 0x0003
     *     getting 8bit(s) from position 2115 --> 0x005c
     *     getting 8bit(s) from position 2123 --> 0x00a0
     *     getting 8bit(s) from position 2131 --> 0x0012
     *     getting 8bit(s) from position 2139 --> 0x0050
     *     getting 8bit(s) from position 2147 --> 0x00bc
     *     getting 8bit(s) from position 2155 --> 0x00c5
     *     getting 8bit(s) from position 2163 --> 0x00f6
     *     getting 8bit(s) from position 2171 --> 0x00c2
     *     getting 8bit(s) from position 2179 --> 0x009f
     *     getting 8bit(s) from position 2187 --> 0x00f8
     *     getting 8bit(s) from position 2195 --> 0x00c8
     *     getting 8bit(s) from position 2203 --> 0x00db
     *     getting 8bit(s) from position 2211 --> 0x0063
     *     getting 8bit(s) from position 2219 --> 0x0023
     *     getting 8bit(s) from position 2227 --> 0x0081
     *     getting 8bit(s) from position 2235 --> 0x003f
     *     getting 8bit(s) from position 2243 --> 0x0080
     *     getting 8bit(s) from position 2251 --> 0x00ce
     *     getting 8bit(s) from position 2259 --> 0x00db
     *     getting 8bit(s) from position 2267 --> 0x00aa
     *     getting 8bit(s) from position 2275 --> 0x0020
     *     getting 8bit(s) from position 2283 --> 0x00b9
     *     getting 8bit(s) from position 2291 --> 0x0072
     *     getting 8bit(s) from position 2299 --> 0x00d8
     *     getting 8bit(s) from position 2307 --> 0x000b
     *     getting 1bit(s) from position 2315 --> 0x0000
     * signatureValue -> Done
     *   Element[START_ELEMENT(KeyInfo), START_ELEMENT(Object), END_ELEMENT]
     *     getting 2bit(s) from position 2316 --> 0x0002
     * SignatureType -> Done
     *   Element[END_ELEMENT]
     *     getting 1bit(s) from position 2318 --> 0x0000
     * End iso header
     */

    std::vector<uint8_t> raw_data({ 0x02, 0x0d, 0xf6, 0xa6, 0x3d, 0xcd, 0x82, 0x24, 0x0f, 0x8a, 0x89, 0x5a, 0x1d,
                                    0x1d, 0x1c, 0x0e, 0x8b, 0xcb, 0xdd, 0xdd, 0xdd, 0xcb, 0x9d, 0xcc, 0xcb, 0x9b, 0xdc,
                                    0x99, 0xcb, 0xd5, 0x14, 0x8b, 0xd8, 0xd8, 0x5b, 0x9b, 0xdb, 0x9a, 0x58, 0xd8, 0x5b,
                                    0x0b, 0x59, 0x5e, 0x1a, 0x4b, 0xd0, 0xd5, 0xa1, 0xd1, 0xd1, 0xc0, 0xe8, 0xbc, 0xbd,
                                    0xdd, 0xdd, 0xdc, 0xb9, 0xdc, 0xcc, 0xb9, 0xbd, 0xc9, 0x9c, 0xbc, 0xc8, 0xc0, 0xc0,
                                    0xc4, 0xbc, 0xc0, 0xd0, 0xbd, 0xe1, 0xb5, 0xb1, 0x91, 0xcd, 0xa5, 0x9c, 0xb5, 0xb5,
                                    0xbd, 0xc9, 0x94, 0x8d, 0x95, 0x8d, 0x91, 0xcd, 0x84, 0xb5, 0xcd, 0xa1, 0x84, 0xc8,
                                    0xd4, 0xd9, 0x10, 0x31, 0x1b, 0x4b, 0x21, 0x88, 0x12, 0xb4, 0x3a, 0x3a, 0x38, 0x1d,
                                    0x17, 0x97, 0xbb, 0xbb, 0xbb, 0x97, 0x3b, 0x99, 0x97, 0x37, 0xb9, 0x33, 0x97, 0xaa,
                                    0x29, 0x17, 0xb1, 0xb0, 0xb7, 0x37, 0xb7, 0x34, 0xb1, 0xb0, 0xb6, 0x16, 0xb2, 0xbc,
                                    0x34, 0x97, 0xa4, 0x29, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x77, 0x77, 0x77,
                                    0x2e, 0x77, 0x33, 0x2e, 0x6f, 0x72, 0x67, 0x2f, 0x32, 0x30, 0x30, 0x31, 0x2f, 0x30,
                                    0x34, 0x2f, 0x78, 0x6d, 0x6c, 0x65, 0x6e, 0x63, 0x23, 0x73, 0x68, 0x61, 0x32, 0x35,
                                    0x36, 0x42, 0x01, 0x4e, 0x3d, 0x87, 0xae, 0x24, 0x24, 0xb6, 0xc3, 0x05, 0xfd, 0x5f,
                                    0x18, 0xa2, 0x4f, 0xe2, 0xf2, 0xf6, 0x87, 0xcd, 0x65, 0x8d, 0x59, 0xbb, 0xe5, 0x1a,
                                    0x7c, 0xe3, 0xf5, 0xcf, 0x08, 0x6f, 0x01, 0x28, 0xc6, 0x08, 0x80, 0x44, 0x0a, 0xd4,
                                    0x50, 0x13, 0xe7, 0xaa, 0x47, 0x44, 0xa6, 0xe6, 0x45, 0x3c, 0xad, 0x9e, 0xb6, 0xa9,
                                    0x6e, 0xd1, 0xb0, 0x7e, 0x44, 0x28, 0x6b, 0xce, 0x1a, 0xe4, 0xce, 0x99, 0xf8, 0xa8,
                                    0xe5, 0x13, 0x20, 0x44, 0x03, 0x8f, 0x21, 0x9b, 0xbe, 0xd1, 0xe0, 0x6b, 0x94, 0x02,
                                    0x4a, 0x17, 0x98, 0xbe, 0xd8, 0x53, 0xff, 0x19, 0x1b, 0x6c, 0x64, 0x70, 0x27, 0xf0,
                                    0x19, 0xdb, 0x75, 0x44, 0x17, 0x2e, 0x5b, 0x01, 0x68});

    std::string input_json = R"({"SessionID":"37DA98F73608903E","Signature":{"SignedInfo":{"CanonicalizationMethod":{"Algorithm":"http://www.w3.org/TR/canonical-exi/"},"SignatureMethod":{"Algorithm":"http://www.w3.org/2001/04/xmldsig-more#ecdsa-sha256"},"Reference":[{"URI":"#id1","Transforms":{"Transform":[{"Algorithm":"http://www.w3.org/TR/canonical-exi/"}]},"DigestMethod":{"Algorithm":"http://www.w3.org/2001/04/xmlenc#sha256"},"DigestValue":"FOPYeuJCS2wwX9XxiiT+Ly9ofNZY1Zu+UafOP1zwhvA="}]},"SignatureValue":{"value":"MEQCIFaigJ89UjolNzIp5Wz1tUt2jYPyIUNecNcmdM/FRyiZAiAceQzd9o8DXKASULzF9sKf+MjbYyOBP4DO26oguXLYCw=="}}})";
    setupWithJsonData(input_json);

    complex_types->encode_MessageHeaderType(json_object);

    compareBinaryVector(bit_stream->get_exi_data(), raw_data);
}

TEST_F(Iso2EncodeComplexTypesTest, EncodeOptionalDerivation_PowerDeliveryReq) {
    /* Data extracted from OpenV2G
     * DecodePowerDeliveryReqType -> Start
     *     getting 1bit(s) from position 100 --> 0x0000
     *     getting 1bit(s) from position 101 --> 0x0000
     *     getting 2bit(s) from position 102 --> 0x0000
     *     getting 1bit(s) from position 104 --> 0x0000
     *     getting 1bit(s) from position 105 --> 0x0000
     *     getting 1bit(s) from position 106 --> 0x0000
     *     getting 8bit(s) from position 107 --> 0x0000
     *     getting 1bit(s) from position 115 --> 0x0000
     *   Element[START_ELEMENT(ChargingProfile), START_ELEMENT(DC_EVPowerDeliveryParameter), START_ELEMENT(EVPowerDeliveryParameter), END_ELEMENT]
     *     getting 3bit(s) from position 116 --> 0x0001
     *   DC_EVPowerDeliveryParameterType -> Start1
     *   FirstStartTag[START_ELEMENT(DC_EVStatus)]
     *     getting 1bit(s) from position 119 --> 0x0000
     *   FirstStartTag[START_ELEMENT(EVReady)]
     *     getting 1bit(s) from position 120 --> 0x0000
     *     getting 1bit(s) from position 121 --> 0x0000
     *     getting 1bit(s) from position 122 --> 0x0001
     *   END_ELEMENT(EVReady)
     *     getting 1bit(s) from position 123 --> 0x0000
     *   Element[START_ELEMENT(EVErrorCode)]
     *     getting 1bit(s) from position 124 --> 0x0000
     *     getting 1bit(s) from position 125 --> 0x0000
     *     getting 4bit(s) from position 126 --> 0x0000
     *   END_ELEMENT(EVErrorCode)
     *     getting 1bit(s) from position 130 --> 0x0000
     *   Element[START_ELEMENT(EVRESSSOC)]
     *     getting 1bit(s) from position 131 --> 0x0000
     *     getting 1bit(s) from position 132 --> 0x0000
     *     getting 7bit(s) from position 133 --> 0x003c
     *   END_ELEMENT(EVRESSSOC)
     *     getting 1bit(s) from position 140 --> 0x0000
     *   Element[END_ELEMENT]
     *     getting 1bit(s) from position 141 --> 0x0000
     *   Element[START_ELEMENT(BulkChargingComplete), START_ELEMENT(ChargingComplete)]
     *     getting 2bit(s) from position 142 --> 0x0001
     *     getting 1bit(s) from position 144 --> 0x0000
     *     getting 1bit(s) from position 145 --> 0x0001
     *     getting 1bit(s) from position 146 --> 0x0000
     *   Element[END_ELEMENT]
     *     getting 1bit(s) from position 147 --> 0x0000
     *   DC_EVPowerDeliveryParameterType -> Done1
     *   Element[END_ELEMENT]
     *     getting 1bit(s) from position 148 --> 0x0000
     * DecodePowerDeliveryReqType -> Done
     */

    std::string input_json = R"({"ChargeProgress":"Start","SAScheduleTupleID":1,"DC_EVPowerDeliveryParameter":{"DC_EVStatus":{"EVReady":true,"EVErrorCode":"NO_ERROR","EVRESSSOC":60},"ChargingComplete":true}})";
    std::vector<uint8_t> output_raw({0b00000000, 0b00000000, 0b00100010, 0b00000000, 0b00111100, 0b00010100});
    setupWithJsonData(input_json);

    complex_types->encode_PowerDeliveryReqType(json_object);

    compareBinaryVector(bit_stream.get()->get_exi_data(), output_raw);
}

TEST_F(Iso2EncodeComplexTypesTest, EncodeList_ChargeParameterDiscoveryRes_PMaxScheduleEntry) {
    /* Data extracted from OpenV2G
     * PMaxScheduleType Start
     *   FirstStartTag[START_ELEMENT(PMaxScheduleEntry)]
     *     getting 1bit(s) from position 2355 --> 0x0000
     *   FirstStartTag[START_ELEMENT(RelativeTimeInterval), START_ELEMENT(TimeInterval)]
     *     getting 2bit(s) from position 2356 --> 0x0000
     *   FirstStartTag[START_ELEMENT(start)]
     *     getting 1bit(s) from position 2358 --> 0x0000
     *   FirstStartTag[CHARACTERS[UNSIGNED_INTEGER]]
     *     getting 1bit(s) from position 2359 --> 0x0000
     *     getting 8bit(s) from position 2360 --> 0x0000
     *   End_ELEMENT(start)
     *     getting 1bit(s) from position 2368 --> 0x0000
     *   Element[START_ELEMENT(duration), END_ELEMENT]
     *     getting 2bit(s) from position 2369 --> 0x0000
     *   FirstStartTag[CHARACTERS[UNSIGNED_INTEGER]]
     *     getting 1bit(s) from position 2371 --> 0x0000
     *     getting 8bit(s) from position 2372 --> 0x0090
     *     getting 8bit(s) from position 2380 --> 0x001c
     *   END_ELEMENT(duration)
     *     getting 1bit(s) from position 2388 --> 0x0000
     *   Element[END_ELEMENT]
     *     getting 1bit(s) from position 2389 --> 0x0000
     *   Element[START_ELEMENT(PMax)]
     *     getting 1bit(s) from position 2390 --> 0x0000
     *   FirstStartTag[START_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Multiplier)]
     *     getting 1bit(s) from position 2391 --> 0x0000
     *   FirstStartTag[CHARACTERS[NBIT_UNSIGNED_INTEGER]]
     *     getting 1bit(s) from position 2392 --> 0x0000
     *     getting 3bit(s) from position 2393 --> 0x0003
     *   END_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Multiplier)
     *     getting 1bit(s) from position 2396 --> 0x0000
     *   Element[START_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Unit)]
     *     getting 1bit(s) from position 2397 --> 0x0000
     *   FirstStartTag[CHARACTERS[ENUMERATION]]
     *     getting 1bit(s) from position 2398 --> 0x0000
     *     getting 3bit(s) from position 2399 --> 0x0005
     *   END_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Unit)
     *     getting 1bit(s) from position 2402 --> 0x0000
     *   Element[START_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Value)]
     *     getting 1bit(s) from position 2403 --> 0x0000
     *   First(xsi:type)StartTag[CHARACTERS[INTEGER]]
     *     getting 1bit(s) from position 2404 --> 0x0000
     *     getting 1bit(s) from position 2405 --> 0x0000
     *     getting 8bit(s) from position 2406 --> 0x00f8
     *     getting 8bit(s) from position 2414 --> 0x0055
     *   END_ELEMENT({urn:iso:15118:2:2013:MsgDataTypes}Value)
     *     getting 1bit(s) from position 2422 --> 0x0000
     *   Element[END_ELEMENT]
     *     getting 1bit(s) from position 2423 --> 0x0000
     *   Element[END_ELEMENT]
     *     getting 1bit(s) from position 2424 --> 0x0000
     *   Element[START_ELEMENT(PMaxScheduleEntry)]
     *     getting 2bit(s) from position 2425 --> 0x0001
     * PMaxScheduleType Done
     */

    std::string input_json = R"({"PMaxScheduleEntry":[{"RelativeTimeInterval":{"start":0,"duration":3600},"PMax":{"Multiplier":0,"Unit":"W","Value":11000}}]})";
    std::vector<uint8_t> output_raw({0b00000000, 0b00000000, 0b01001000, 0b00001110, 0b00000001, 0b10001010, 0b00011111,
                                     0b00001010, 0b10100001});
    setupWithJsonData(input_json);

    complex_types->encode_PMaxScheduleType(json_object);

    compareBinaryVector(bit_stream.get()->get_exi_data(), output_raw);
}

TEST_F(Iso2EncodeComplexTypesTest, EncodeList_ChargeParameterDiscoveryRes_SalesTariff) {
    /* Data extracted from OpenV2G
     * SalesTariffType Start
     *   FirstStartTag[ATTRIBUTE[STRING](Id), START_ELEMENT(SalesTariffID)]
     *     getting 2bit(s) from position 2429 --> 0x0000
     *   Id
     *     getting 8bit(s) from position 2431 --> 0x0005
     *     getting 8bit(s) from position 2439 --> 0x0069
     *     getting 8bit(s) from position 2447 --> 0x0064
     *     getting 8bit(s) from position 2455 --> 0x0031
     *   StartTag[START_ELEMENT(SalesTariffID)]
     *     getting 1bit(s) from position 2463 --> 0x0000
     *     getting 1bit(s) from position 2464 --> 0x0000
     *     getting 8bit(s) from position 2465 --> 0x0009
     *     getting 1bit(s) from position 2473 --> 0x0000
     *   Element[START_ELEMENT(SalesTariffDescription), START_ELEMENT(NumEPriceLevels), START_ELEMENT(SalesTariffEntry)]
     *     getting 2bit(s) from position 2474 --> 0x0001
     *   START_ELEMENT(NumEPriceLevels)
     *     getting 1bit(s) from position 2476 --> 0x0000
     *     getting 8bit(s) from position 2477 --> 0x0002
     *     getting 1bit(s) from position 2485 --> 0x0000
     *   Element[START_ELEMENT(SalesTariffEntry)]
     *     getting 1bit(s) from position 2486 --> 0x0000
     *   START_ELEMENT(SalesTariffEntry)
     *     getting 2bit(s) from position 2487 --> 0x0000
     *   FirstStartTag[START_ELEMENT(start)]
     *     getting 1bit(s) from position 2489 --> 0x0000
     *   FirstStartTag[CHARACTERS[UNSIGNED_INTEGER]]
     *     getting 1bit(s) from position 2490 --> 0x0000
     *     getting 8bit(s) from position 2491 --> 0x0000
     *   End_ELEMENT(start)
     *     getting 1bit(s) from position 2499 --> 0x0000
     *   Element[START_ELEMENT(duration), END_ELEMENT]
     *     getting 2bit(s) from position 2500 --> 0x0001
     *     getting 2bit(s) from position 2502 --> 0x0000
     *     getting 1bit(s) from position 2504 --> 0x0000
     *     getting 8bit(s) from position 2505 --> 0x0001
     *     getting 1bit(s) from position 2513 --> 0x0000
     *     getting 2bit(s) from position 2514 --> 0x0001
     *   Element[START_ELEMENT(SalesTariffEntry), END_ELEMENT]
     *     getting 2bit(s) from position 2516 --> 0x0000
     *     getting 2bit(s) from position 2518 --> 0x0000
     *   FirstStartTag[START_ELEMENT(start)]
     *     getting 1bit(s) from position 2520 --> 0x0000
     *   FirstStartTag[CHARACTERS[UNSIGNED_INTEGER]]
     *     getting 1bit(s) from position 2521 --> 0x0000
     *     getting 8bit(s) from position 2522 --> 0x0089
     *     getting 8bit(s) from position 2530 --> 0x000e
     *   End_ELEMENT(start)
     *     getting 1bit(s) from position 2538 --> 0x0000
     *   Element[START_ELEMENT(duration), END_ELEMENT]
     *     getting 2bit(s) from position 2539 --> 0x0000
     *   FirstStartTag[CHARACTERS[UNSIGNED_INTEGER]]
     *     getting 1bit(s) from position 2541 --> 0x0000
     *     getting 8bit(s) from position 2542 --> 0x0087
     *     getting 8bit(s) from position 2550 --> 0x000e
     *   END_ELEMENT(duration)
     *     getting 1bit(s) from position 2558 --> 0x0000
     *   Element[END_ELEMENT]
     *     getting 1bit(s) from position 2559 --> 0x0000
     *     getting 2bit(s) from position 2560 --> 0x0000
     *     getting 1bit(s) from position 2562 --> 0x0000
     *     getting 8bit(s) from position 2563 --> 0x0002
     *     getting 1bit(s) from position 2571 --> 0x0000
     *     getting 2bit(s) from position 2572 --> 0x0001
     *   Element[START_ELEMENT(SalesTariffEntry), END_ELEMENT]
     *     getting 2bit(s) from position 2574 --> 0x0001
     * SalesTariffType Done
     */

    std::string input_json = R"({"Id":"id1","SalesTariffID":10,"NumEPriceLevels":2,"SalesTariffEntry":[{"RelativeTimeInterval":{"start":0},"EPriceLevel":1},{"RelativeTimeInterval":{"start":1801,"duration":1799},"EPriceLevel":2}]})";
    std::vector<uint8_t> output_raw({0b00000001, 0b01011010, 0b01011001, 0b00001100, 0b01000000, 0b10010010,
                                     0b00000010, 0b00000000, 0b00000000, 0b10000000, 0b00010010, 0b00000100,
                                     0b01001000, 0b01110000, 0b01000011, 0b10000111, 0b00000000, 0b00001000,
                                     0b10100000});
    setupWithJsonData(input_json);

    complex_types->encode_SalesTariffType(json_object);

    compareBinaryVector(bit_stream.get()->get_exi_data(), output_raw);
}
