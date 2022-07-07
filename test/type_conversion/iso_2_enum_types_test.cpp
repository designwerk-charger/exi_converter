#include "type_conversion/base_types.h"
#include "iso15118_2/enum_types.h"
#include "base/output_string_stream.h"
#include "mock/mock_input_string_stream.h"
#include "mock/mock_base_types.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"


TEST(Iso2EnumTypesTest, decode_responseCodeType) {
    uint8_t data;
    BitStream bs(&data, 1);
    MockBaseTypes mock(&bs);
    iso15118_2::EnumTypes et(&mock);
    ON_CALL(mock, extractNBitsForEnum).WillByDefault(::testing::Return(10));

    std::string result = et.decode_responseCodeType();

    ASSERT_EQ(result, "FAILED_CertificateExpired");
}

TEST(Iso2EnumTypesTest, encode_responseCodeType) {
    MockInputStringStream miss;
    BitStream bs;
    MockBaseTypes mock(&bs);
    iso15118_2::EnumTypes et(&mock, &miss);

    EXPECT_CALL(miss, get_item_and_move_to_next()).WillOnce(testing::Return("FAILED_CertificateExpired"));
    EXPECT_CALL(mock, injectNBitsForEnum(10, 5)).Times(1);

    et.encode_responseCodeType();
}

TEST(Iso2EnumTypesTest, encode_throws_exception_when_responseCodeType_not_found) {
    MockInputStringStream miss;
    BitStream bs;
    MockBaseTypes mock(&bs);
    iso15118_2::EnumTypes et(&mock, &miss);

    EXPECT_CALL(miss, get_item_and_move_to_next()).WillOnce(testing::Return("ValueNotInEnum"));

    ASSERT_THROW(et.encode_responseCodeType(), std::runtime_error);
}
