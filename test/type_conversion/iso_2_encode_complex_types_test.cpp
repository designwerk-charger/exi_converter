#include "type_conversion/base_types.h"
#include "iso15118_2/complex_types.h"

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
