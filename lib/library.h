#ifndef EXI_CONVERTER_LIBRARY_H
#define EXI_CONVERTER_LIBRARY_H

#include <iostream>


class ExiCodec{
public:
    ExiCodec();

    std::string decode(char* byte_stream, std::string ns);

    char * encode(std::string json_str, std::string ns);
};



#endif //EXI_CONVERTER_LIBRARY_H
