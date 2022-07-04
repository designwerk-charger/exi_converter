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

