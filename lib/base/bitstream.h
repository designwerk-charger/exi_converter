#ifndef EXI_CONVERTER_BITSTREAM_H
#define EXI_CONVERTER_BITSTREAM_H


#include <cstdint>
#include <cstdio>
#include <vector>

class BitStream {
public:
    BitStream(uint8_t * input_data, size_t length_byte);
    BitStream(const std::vector<uint8_t> & input_data);
    BitStream();

    void get_next_n_bits(uint32_t bitsrequested, uint8_t * data);
    uint32_t get_max_4bytes(uint8_t bitsrequested);

    void add_max_8bits(uint8_t data, uint8_t num_bits);
    void add_bytes(const uint8_t * data, uint8_t num_bytes);
    std::vector<uint8_t> get_exi_data();
private:
    std::vector<uint8_t>exi_data_;
    uint32_t bit_counter_;
    uint32_t num_bits_;
};

#endif //EXI_CONVERTER_BITSTREAM_H
