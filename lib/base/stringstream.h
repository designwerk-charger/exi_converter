#ifndef EXI_CONVERTER_STRINGSTREAM_H
#define EXI_CONVERTER_STRINGSTREAM_H

#include <cstdint>
#include <cstdio>

class StringStream {
public:
    StringStream();

    uint32_t get_next_n_bits(uint8_t bits);

    void add_n_bits(uint8_t bits, uint32_t data);
};


#endif //EXI_CONVERTER_STRINGSTREAM_H
