#include "type_conversion/base_types.h"


#include "gmock/gmock.h"
#include "gtest/gtest.h"

TEST(ExiLibTest, DummyConversionTest) {
    uint8_t raw_data = 8;
    BitStream bs(&raw_data, 1);

    EXPECT_EQ(0, decodeInteger(bs, 4));
}
