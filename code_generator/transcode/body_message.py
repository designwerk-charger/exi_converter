import collections
from typing import Dict

from xmlschema import XsdElement

from cpp.cpp_class import CppClass
from cpp.cpp_function import CppFunction
from transcode.complex_types import ComplexTypes


class BodyMessage:

    @staticmethod
    def get_elements_with_type_derived_from_body_base_sorted_lexicographically(schema) -> Dict[int, XsdElement]:
        element_top = {}
        for elem in schema.elements.target_dict.values():
            if elem.type.local_name == "BodyBaseType":
                element_top[elem.local_name] = elem
            elif elem.type.base_type and elem.type.base_type.local_name == "BodyBaseType":
                element_top[elem.local_name] = elem

        sorted_elements = collections.OrderedDict(sorted(element_top.items()))

        numbred_elements = {}
        for i, msg_key in enumerate(sorted_elements.keys()):
            numbred_elements[i] = sorted_elements[msg_key]
        return numbred_elements

    def __init__(self, schema, complex_types: ComplexTypes, namespace=None):
        self.complex_type_names = []
        for t in complex_types.all_complex_types:
            self.complex_type_names.append(t.base_type_name)

        relevant_body_elements_numbered = self.get_elements_with_type_derived_from_body_base_sorted_lexicographically(schema)

        self.cpp_class = CppClass(class_name="BodyMessage", derived_from_class=None,
                                  includes="#include <utility>\n#include <unordered_map>\n#include \"complex_types.h\"\n#include \"base/bitstream.h\"\n#include \"base/output_string_stream.h\"\n",
                                  namespace=namespace)
        self.cpp_class.add_member("ComplexTypes * complex_types_;\n\tBitStream * bit_stream_;\n"
                                  "\tOutputStringStream * output_string_stream_;\n\tInputStringStream * input_string_stream_;")
        self.cpp_class.add_constructor("ComplexTypes * complex_types, BitStream * bit_stream, OutputStringStream * output_string_stream",
                                       "complex_types_ = complex_types;\nbit_stream_ = bit_stream;\n"
                                       "output_string_stream_ = output_string_stream;\ninput_string_stream_ = nullptr;\n")
        self.cpp_class.add_constructor("ComplexTypes * complex_types, BitStream * bit_stream, InputStringStream * input_string_stream",
                                       "complex_types_ = complex_types;\nbit_stream_ = bit_stream;\n"
                                       "output_string_stream_ = nullptr;\ninput_string_stream_ = input_string_stream;\n")

        self.cpp_class.add_function(self.getDecodeFunction(relevant_body_elements_numbered))
        self.cpp_class.add_function(self.getEncodeFunction(relevant_body_elements_numbered))


    def getDecodeFunction(self, elements) -> CppFunction:
        code = f"uint8_t message_type;\n" \
               f"bit_stream_->get_next_n_bits(6, &message_type);\n" \
               f"switch(message_type) ""{\n"

        for key, value in elements.items():
            if value.type.local_name in self.complex_type_names:
                code += f"\tcase {key}:\n" \
                        f"\t\t#ifndef NDEBUG\n" \
                        f"\t\tstd::cout << \"decoding body as type '{value.type.local_name}' -> {key}\" << std::endl;\n" \
                        f"\t\t#endif\n" \
                        f"\t\toutput_string_stream_->start_key(\"{value.local_name}\");\n" \
                        f"\t\tcomplex_types_->decode_{value.type.local_name}();\n" \
                        f"\t\tbreak;\n"
            else:
                code += f"\tcase {key}: throw std::runtime_error(\"The type {value.type.local_name} with number {key}" \
                        f", can not be directly decoded!\");\n"

        code += f"\tdefault:\n" \
                f"\t\tstd::ostringstream stm;\n" \
                f"\t\tstm << \"The requested value \" << message_type << \" is out of range!\";\n" \
                f"\t\tthrow std::runtime_error(stm.str());\n" \
                f"}};\n" \
                f"output_string_stream_->end_key();\n"

        return CppFunction(function_name="decodeBody",
                           return_type="void",
                           arguments=None,
                           code=code,
                           comment=None)

    def getEncodeFunction(self, elements) -> CppFunction: #void (foo::*method)()
        code = f"static std::unordered_map<std::string, std::pair<uint32_t, void(ComplexTypes::*)(const std::shared_ptr<JObject> &)>> const table = {{\n"
        for key, value in elements.items():
            if value.type.local_name in self.complex_type_names:
                code += f"\t{{\"{value.local_name}\", std::make_pair({key}, &ComplexTypes::encode_{value.type.local_name})}},\n"
        code += f"}};\n\n"

        code += f"const auto & msg_name = body->get_next_key_name();\n" \
                f"auto it = table.find(msg_name);\n" \
                f"if (it != table.end()) {{\n" \
                "\tauto message_nr = it->second.first;\n" \
                "\tauto message_fcn = it->second.second;\n" \
                "\tbit_stream_->add_max_8bits(message_nr, 6);\n" \
                "\t(complex_types_->*message_fcn)(body);\n" \
                f"}} else {{\n" \
                f"\tthrow std::runtime_error(\"The message with name \" + msg_name + \" can not be found\");\n" \
                f"}}\n"

        return CppFunction(function_name="encodeBody",
                           return_type="void",
                           arguments="std::shared_ptr<JObject> body",
                           code=code,
                           comment=None)

    def write_body_conversion_header(self, directory: str):
        self.cpp_class.write_h(directory)

    def write_body_conversion_source(self, directory: str):
        self.cpp_class.write_cpp(directory)
