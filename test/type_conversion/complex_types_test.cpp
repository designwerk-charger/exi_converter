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
    std::vector<uint8_t> raw_data({0b00000000, 0b10000000, 0b00000000, 0b00000010, 0b00000100});
    setupWithRawData(raw_data);

    complex_types->decode_PowerDeliveryResType();

    ASSERT_EQ(string_stream->get_full_stream(), R"({"ResponseCode":"OK","DC_EVSEStatus":{"NotificationMaxDelay":0,"EVSENotification":"None","EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready"}})");
}


TEST_F(ComplexTypesTest, DecodeElementsList_PaymentServiceSelectionReqType_SelectedServiceList) {
    std::vector<uint8_t> raw_data({0b00000000, 0b00100101, 0x00});
    setupWithRawData(raw_data);

    complex_types->decode_SelectedServiceListType();

    // {"PaymentServiceSelectionReq":{"SelectedPaymentOption":"ExternalPayment","SelectedServiceList":{"SelectedService":[{"ServiceID":1}]}}}}}
    ASSERT_EQ(string_stream->get_full_stream(), R"({"SelectedService":[{"ServiceID":1}]})");
}
