#ifndef EXI_CONVERTER_BITSTREAM_H
#define EXI_CONVERTER_BITSTREAM_H


#include <cstdint>
#include <cstdio>
#include <vector>

class BitStream {
public:
    BitStream(uint8_t * input_data, size_t length);
    BitStream(const std::vector<uint8_t> & input_data);

    void get_next_n_bits(uint32_t bitsrequested, uint8_t * data);

    void add_n_bits(uint8_t bits, uint8_t * data);

    // Subfunctions
    uint32_t get_max_4bytes(uint8_t bitsrequested);

private:
    size_t length_;
    uint8_t * exi_data_;
    uint32_t bit_counter_;
    uint32_t num_bits_;
};

#endif //EXI_CONVERTER_BITSTREAM_H
