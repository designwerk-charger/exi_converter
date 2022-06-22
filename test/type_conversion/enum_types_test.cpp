#include "type_conversion/base_types.h"
#include "iso15118_2/enum_types.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"


class MockBaseTypes : public BaseTypes {
 public:
    MockBaseTypes(BitStream * bs) : BaseTypes(bs) {}  // NOLINT
    // uint32_t BaseTypes::extractNBitsForEnum(uint32_t n_bits)
    MOCK_METHOD(uint32_t , extractNBitsForEnum, (uint32_t), (override));
};

TEST(EnumTypesTest, decode_responseCodeType) {
    uint8_t data;
    BitStream bs(&data, 1);
    MockBaseTypes mock(&bs);
    iso15118_2::EnumTypes et(&mock);
    ON_CALL(mock, extractNBitsForEnum).WillByDefault(::testing::Return(10));

    std::string result = et.decode_responseCodeType();

    ASSERT_EQ(result, "FAILED_CertificateExpired");
}

