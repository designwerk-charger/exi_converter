#ifndef EXI_CONVERTER_LIBRARY_H
#define EXI_CONVERTER_LIBRARY_H

#include <iostream>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>

class ExiCodec{
public:
    ExiCodec();

    std::string decode(const std::vector<uint8_t> & byte_stream, std::string ns);
    std::vector<uint8_t> encode(const std::string& json_str, const std::string& ns);

    std::string py_decode(const char * data, uint32_t length, const std::string& ns);
    pybind11::bytes py_encode(const std::string& json_str, const std::string& ns);
};



#endif //EXI_CONVERTER_LIBRARY_H
