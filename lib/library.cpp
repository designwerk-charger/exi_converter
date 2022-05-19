#include "library.h"
#include <pybind11/pybind11.h>

#include <iostream>

static std::string getStringCanIf() {
    return "blabla";
}

// the module name "exi_converter" must be the same as the one in the cmake file
PYBIND11_MODULE(exi_converter, m) {
    m.doc() = R"pbdoc(
        Designwerk EXI Converter Library
        -----------------------

        Converts JSON to EXI and backword acording to xsd specivifaction for given namespace
    )pbdoc";

    // VERSION_INFO provided by setup.py installation script
#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
    m.def("getStringCanIf", &getStringCanIf); //

    pybind11::class_<ExiCodec>(m, "ExiCodec", "Class to encode and decode EXI")
            .def(pybind11::init<>())
            .def("decode", &ExiCodec::decode, "Decode EXI to json")
            .def("encode", &ExiCodec::encode, "Enocode EXI from json");
}

char* ExiCodec::encode(std::string json_str, std::string ns) {
    std::cout << "input string: "<< json_str << std::endl;
    std::cout << "function encode ..." << std::endl;
    return (char*)"RETURN_BYTE_STREAM_OF_ENCODED_EXI";
}

std::string ExiCodec::decode(char* byte_stream, std::string ns) {
    std::cout << "input byte-stream: "<< byte_stream << std::endl;
    std::cout << "function decode ..." << std::endl;
    return "return_json_of_decoded_EXI";
}

ExiCodec::ExiCodec() {

}
