#include "library.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <iostream>
#include "base/bitstream.h"
#include "type_conversion/base_types.h"

#include "iso15118_2/complex_types.h"
#include "iso15118_2/body_message.h"
#include "din_spec/complex_types.h"
#include "din_spec/body_message.h"
#include "app_protocol/complex_types.h"
#include "app_protocol/body_message.h"



// the module name "exi_converter" must be the same as the one in the cmake file
PYBIND11_MODULE(exi_converter, m) {
    m.doc() = R"pbdoc(
        Designwerk EXI Converter Library
        -----------------------

        Converts JSON to EXI and backword acording to xsd specivifaction for given namespace
    )pbdoc";

    // VERSION_INFO provided by setup.py installation script
    std::string str_dbg = "";
    #ifndef NDEBUG
    str_dbg = "-debug";
    #endif
#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO + str_dbg;
#else
    m.attr("__version__") = "dev" + str_dbg;
#endif

    //m.def("pydecode", [](char* c, uint32_t length, std::string ns){py_decode((uint8_t *)c, length, ns);}, "Decode EXI to json");
    //m.def("pydecode", py_decode, "Decode EXI to json");

    pybind11::class_<ExiCodec>(m, "ExiCodec", "Class to encode and decode EXI")
            .def(pybind11::init<>())
            .def("decode", &ExiCodec::py_decode, "Decode EXI to json")
            .def("encode", &ExiCodec::encode, "Enocode EXI from json");
}


std::vector<uint8_t> encode_iso15118_2(InputStringStream * stringstream, BitStream * bitstream) {
    BaseTypes base_types(bitstream, stringstream);
    iso15118_2::EnumTypes enums(&base_types, stringstream);
    iso15118_2::ComplexTypes complex_types(&base_types, &enums, stringstream);
    iso15118_2::BodyMessage body_message(&complex_types, bitstream, stringstream);

    bitstream->add_max_8bits(76, 7);

    stringstream->verify_item_and_move_to_next("V2G_Message");
    base_types.add_event_code("V2G_Message");

    stringstream->verify_item_and_move_to_next("Header");
    complex_types.encode_MessageHeaderType();


    auto body = stringstream->get_item_and_move_to_next();
    if (body != "Body") {
        throw std::runtime_error("Found \"" + body + "\" instead of \"Body\" item found in "
                                 + stringstream->get_input_data());
    }
    base_types.add_event_code("Body");
    body_message.encodeBody();


    return bitstream->get_exi_data();
}

std::string encode_din_spec(InputStringStream * stringstream, BitStream * bitstream) {
    return "";
}

std::vector<uint8_t> encode_app_protocol(InputStringStream * stringstream, BitStream * bitstream) {
    BaseTypes base_types(bitstream, stringstream);
    app_protocol::EnumTypes enums(&base_types, stringstream);
    app_protocol::ComplexTypes complex_types(&base_types, &enums, stringstream);
    app_protocol::BodyMessage body_message(&complex_types, bitstream, stringstream);

    auto message_id = stringstream->get_item_and_move_to_next();
    if (message_id == "supportedAppProtocolReq") {
        bitstream->add_max_8bits(0, 2);
        complex_types.encode_supportedAppProtocolReqType();
    } else if (message_id == "supportedAppProtocolRes") {
        bitstream->add_max_8bits(1, 2);
        complex_types.encode_supportedAppProtocolResType();
    } else {
        throw std::runtime_error("The AppProtocol is unknown (" + message_id + ")!");
    }

    return bitstream->get_exi_data();
}

std::vector<uint8_t> ExiCodec::encode(std::string json_str, std::string ns) {
    std::cout << "input string: "<< json_str << std::endl;
    std::cout << "function encode ..." << std::endl;


    InputStringStream stringstream(json_str);
    BitStream bitstream;

    // adding exi options
    bitstream.add_max_8bits(0x80, 8);

    if (ns == "urn:iso:15118:2:2013:MsgDef") {
        return encode_iso15118_2(&stringstream, &bitstream);
    } else if (ns == "urn:din:70121:2012:MsgDef") {
        // Todo
        encode_din_spec(&stringstream, &bitstream);
    } else if (ns == "urn:iso:15118:2:2010:AppProtocol") {
        return encode_app_protocol(&stringstream, &bitstream);
    }
    throw std::runtime_error("The namespace '" + ns + "' is unknown!");
}

std::string ExiCodec::py_decode(const char * data, uint32_t length, std::string ns) {
    return decode(std::vector<uint8_t>(data, data+length), ns);
}


std::string decode_iso15118_2(BitStream *bitstream) {
    uint8_t message_id;
    bitstream->get_next_n_bits(7, &message_id);
    if (message_id != 76) {
        throw std::runtime_error("V2G_Message with ID 76 was expected! Instead got ID " + std::to_string(message_id));
    }

    BaseTypes base_types(bitstream);
    OutputStringStream stringstream;
    iso15118_2::EnumTypes enums(&base_types);
    iso15118_2::ComplexTypes complex_types(&base_types, &enums, &stringstream);
    iso15118_2::BodyMessage body_message(&complex_types, bitstream, &stringstream);

    stringstream.start_key("V2G_Message");
    base_types.check_event_code_is_0("StartV2G_Message");
    stringstream.start_key("Header");
    complex_types.decode_MessageHeaderType();
    stringstream.end_key(); // header

    stringstream.start_key("Body");
    base_types.check_event_code_is_0("Body");
    body_message.decodeBody();
    stringstream.end_key(); // body

    stringstream.end_key();
    return stringstream.get_full_stream();
}


std::string decode_din_spec(BitStream *bitstream) {
    uint8_t message_id;
    bitstream->get_next_n_bits(7, &message_id);
    if (message_id != 77) {
        throw std::runtime_error("V2G_Message with ID 77 was expected! Instead got ID " + std::to_string(message_id));
    }

    BaseTypes base_types(bitstream);
    OutputStringStream stringstream;
    din_spec::EnumTypes enums(&base_types);
    din_spec::ComplexTypes complex_types(&base_types, &enums, &stringstream);
    din_spec::BodyMessage body_message(&complex_types, bitstream, &stringstream);

    stringstream.start_key("V2G_Message");
    base_types.check_event_code_is_0("StartV2G_Message");
    stringstream.start_key("Header");
    complex_types.decode_MessageHeaderType();
    stringstream.end_key(); // header

    stringstream.start_key("Body");
    base_types.check_event_code_is_0("Body");
    body_message.decodeBody();
    stringstream.end_key(); // body

    stringstream.end_key();
    return stringstream.get_full_stream();
}

std::string decode_app_protocol(BitStream *bitstream) {
    BaseTypes base_types(bitstream);
    OutputStringStream stringstream;
    app_protocol::EnumTypes enums(&base_types);
    app_protocol::ComplexTypes complex_types(&base_types, &enums, &stringstream);
    app_protocol::BodyMessage body_message(&complex_types, bitstream, &stringstream);

    uint8_t message_id;
    bitstream->get_next_n_bits(2, &message_id);
    if (message_id == 0) {
        stringstream.start_key("supportedAppProtocolReq");
        complex_types.decode_supportedAppProtocolReqType();
    } else if (message_id == 1) {
        stringstream.start_key("supportedAppProtocolRes");
        complex_types.decode_supportedAppProtocolResType();
    } else {
        throw std::runtime_error("The event code for AppProtocolSelection is unknown (" + std::to_string(message_id) + ")!");
    }
    stringstream.end_key();
    return stringstream.get_full_stream();
}

std::string ExiCodec::decode(const std::vector<uint8_t> & byte_stream, std::string ns) {
    BitStream bitstream(byte_stream);

    // checking header byte
    uint8_t exi_options;
    bitstream.get_next_n_bits(8, &exi_options);

    if (ns == "urn:iso:15118:2:2013:MsgDef") {
        return decode_iso15118_2(&bitstream);
    } else if (ns == "urn:din:70121:2012:MsgDef") {
        return decode_din_spec(&bitstream);
    } else if (ns == "urn:iso:15118:2:2010:AppProtocol") {
        return decode_app_protocol(&bitstream);
    }
    throw std::runtime_error("The namespace '" + ns + "' is unknown!");
}

ExiCodec::ExiCodec() { }
