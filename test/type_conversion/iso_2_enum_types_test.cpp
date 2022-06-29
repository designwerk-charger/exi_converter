#include "type_conversion/base_types.h"
#include "iso15118_2/enum_types.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "base/stringstream.h"


class MockBaseTypes : public BaseTypes {
 public:
    MockBaseTypes(BitStream * bs) : BaseTypes(bs) {}  // NOLINT
    // uint32_t BaseTypes::extractNBitsForEnum(uint32_t n_bits)
    MOCK_METHOD(uint32_t , extractNBitsForEnum, (uint32_t), (override));

    // void injectNBitsForEnum(uint8_t value, uint8_t n_bits);
    MOCK_METHOD(void , injectNBitsForEnum, (uint8_t, uint8_t), (override));
};

TEST(Iso2EnumTypesTest, decode_responseCodeType) {
    uint8_t data;
    BitStream bs(&data, 1);
    MockBaseTypes mock(&bs);
    StringStream ss("");
    iso15118_2::EnumTypes et(&mock, &ss);
    ON_CALL(mock, extractNBitsForEnum).WillByDefault(::testing::Return(10));

    std::string result = et.decode_responseCodeType();

    ASSERT_EQ(result, "FAILED_CertificateExpired");
}

TEST(Iso2EnumTypesTest, encode_responseCodeType) {
    StringStream ss("{FAILED_CertificateExpired}");
    BitStream bs;
    MockBaseTypes mock(&bs);
    iso15118_2::EnumTypes et(&mock, &ss);

    EXPECT_CALL(mock, injectNBitsForEnum(10, 5)).Times(1);

    et.encode_responseCodeType();
}

TEST(Iso2EnumTypesTest, encode_throws_exception_when_responseCodeType_not_found) {
    StringStream ss("{ValueNotInEnum}");
    BitStream bs;
    MockBaseTypes mock(&bs);
    iso15118_2::EnumTypes et(&mock, &ss);

    ASSERT_THROW(et.encode_responseCodeType(), std::runtime_error);
}
