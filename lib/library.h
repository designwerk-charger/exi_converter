#ifndef EXI_CONVERTER_LIBRARY_H
#define EXI_CONVERTER_LIBRARY_H

#include <iostream>


class ExiCodec{
public:
    ExiCodec();

    std::string decode(uint8_t* byte_stream, uint32_t length, std::string ns);

    char * encode(std::string json_str, std::string ns);
};



#endif //EXI_CONVERTER_LIBRARY_H
