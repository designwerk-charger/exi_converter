#ifndef EXI_CONVERTER_LIBRARY_H
#define EXI_CONVERTER_LIBRARY_H

#include <iostream>
#include <vector>


class ExiCodec{
public:
    ExiCodec();

    std::string decode(const std::vector<uint8_t> & byte_stream, std::string ns);
    std::vector<uint8_t> encode(std::string json_str, std::string ns);

    std::string py_decode(const char * data, uint32_t length, std::string ns);
};



#endif //EXI_CONVERTER_LIBRARY_H
