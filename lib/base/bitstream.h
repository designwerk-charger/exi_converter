#ifndef EXI_CONVERTER_BITSTREAM_H
#define EXI_CONVERTER_BITSTREAM_H


#include <cstdint>
#include <cstdio>

class BitStream {
public:
    BitStream(uint8_t * input_data, size_t length);

    void get_next_n_bits(uint8_t bits, uint8_t * data);

    void add_n_bits(uint8_t bits, uint8_t * data);

private:
    size_t length_;
    uint8_t * exi_data_;
    uint32_t bit_counter_;
    uint32_t num_bits_;
};

#endif //EXI_CONVERTER_BITSTREAM_H
