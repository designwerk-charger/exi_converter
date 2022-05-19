#ifndef EXI_CONVERTER_BITSTREAM_H
#define EXI_CONVERTER_BITSTREAM_H


#include <cstdint>
#include <cstdio>

class BitStream {
public:
    BitStream(uint8_t * input_data, size_t length);

    uint32_t get_next_n_bits(uint8_t bits);

    void add_n_bits(uint8_t bits, uint32_t data);
};

#endif //EXI_CONVERTER_BITSTREAM_H
