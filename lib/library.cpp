#include "library.h"
#include <pybind11/pybind11.h>

#include <iostream>
#include "base/bitstream.h"
#include "complex_types.h"
#include "base_types.h"
#include "body_message.h"

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
    m.def("getStringCanIf", &getStringCanIf);

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

std::string ExiCodec::decode(std::vector<uint8_t> byte_stream,  std::string ns) {
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
    BodyMessage body_message(&complex_types, &bitstream);

    stringstream.start_key("V2G_Message");
    stringstream.start_key("Header");
    complex_types.decode_MessageHeaderType();
    stringstream.end_key();

    stringstream.start_key("Body");
    body_message.decodeBody();
    stringstream.end_key();

    stringstream.end_key();
    return stringstream.get_full_stream();
}

ExiCodec::ExiCodec() {

}
