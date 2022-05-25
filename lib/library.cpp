#include "library.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <iostream>
#include "base/bitstream.h"
#include "complex_types.h"
#include "base_types.h"
#include "body_message.h"



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

    //m.def("pydecode", [](char* c, uint32_t length, std::string ns){py_decode((uint8_t *)c, length, ns);}, "Decode EXI to json");
    //m.def("pydecode", py_decode, "Decode EXI to json");

    pybind11::class_<ExiCodec>(m, "ExiCodec", "Class to encode and decode EXI")
            .def(pybind11::init<>())
            .def("decode", &ExiCodec::py_decode, "Decode EXI to json")
            .def("encode", &ExiCodec::encode, "Enocode EXI from json");
}

char* ExiCodec::encode(std::string json_str, std::string ns) {
    std::cout << "input string: "<< json_str << std::endl;
    std::cout << "function encode ..." << std::endl;
    return (char*)"RETURN_BYTE_STREAM_OF_ENCODED_EXI";
}

std::string ExiCodec::py_decode(const char * data, uint32_t length, std::string ns) {
    return decode(std::vector<uint8_t>(data, data+length), ns);
}

std::string ExiCodec::decode(const std::vector<uint8_t> & byte_stream, std::string ns) {
    BitStream bitstream(byte_stream);

    // checking header byte
    uint8_t exi_options;
    bitstream.get_next_n_bits(8, &exi_options);
    std::cout << "decoding message " << ns << " with options 0x" << std::hex << int(exi_options) << std::endl;

    uint8_t message_id;
    bitstream.get_next_n_bits(7, &message_id);
    if (message_id != 76) {
        std::cerr << "V2G_Message with ID 76 was expected! Instead got ID " << int(message_id) << std::endl;
    }

    BaseTypes base_types(&bitstream);
    EnumTypes enums(&base_types);
    StringStream stringstream("");
    ComplexTypes complex_types(&base_types, &enums, &stringstream);
    BodyMessage body_message(&complex_types, &bitstream, &stringstream);

    stringstream.start_key("V2G_Message");
    base_types.check_event_code_is_0("StartV2G_Message");
    stringstream.start_key("Header");
    base_types.check_event_code_is_0("StartHeader");
    complex_types.decode_MessageHeaderType();
    //base_types.check_event_code_is_0("EndHeader");
    stringstream.end_key(); // header

    stringstream.start_key("Body");
    base_types.check_event_code_is_0("Body");
    body_message.decodeBody();
    stringstream.end_key(); // body

    stringstream.end_key();
    return stringstream.get_full_stream();
}

ExiCodec::ExiCodec() { }
