#include "base_types.h"

#include "gtest/gtest.h"

TEST(ExiLibTest, DummyConversionTest) {
    uint8_t raw_data = 8;
    BitStream bs(&raw_data, 1);
}
