import collections
from typing import Dict, List

from xmlschema import XsdElement

from cpp.cpp_class import CppClass
from cpp.cpp_function import CppFunction
from datatypes.complex_type import ComplexType
from dom.complex_types import ComplexTypes


class BodyMessage:

    @staticmethod
    def get_top_elements_sorted_lexicographically(schema) -> Dict[int, XsdElement]:
        element_top = {}
        for name, element in schema.elements.target_dict.items():
            # Extract all relevant schema entries for vehicle communication
            if element.target_namespace != "http://www.w3.org/2001/XMLSchema":
                element_top[element.local_name] = element

        sorted_elements = collections.OrderedDict(sorted(element_top.items()))

        numbred_elements = {}
        for i, msg_key in enumerate(sorted_elements.keys()):
            numbred_elements[i] = sorted_elements[msg_key]
        return numbred_elements

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

    def __init__(self, schema, complex_types: ComplexTypes):
        self.complex_type_names = []
        for t in complex_types.all_complex_types:
            self.complex_type_names.append(t.type_name)

        relevant_body_elements_numbered = self.get_elements_with_type_derived_from_body_base_sorted_lexicographically(schema)

        self.cpp_class = CppClass(class_name="BodyMessage", derived_from_class=None,
                                  includes="#include \"complex_types.h\"\n#include \"base/bitstream.h\"\n#include \"base/stringstream.h\"\n")
        self.cpp_class.add_member("ComplexTypes * complex_types_;\n\tBitStream * bit_stream_;\n\tStringStream * string_stream_;")
        self.cpp_class.add_constructor("ComplexTypes * complex_types, BitStream * bit_stream, StringStream * string_stream",
                                       "complex_types_ = complex_types;\n\tbit_stream_ = bit_stream;\n\tstring_stream_ = string_stream;\n")

        self.cpp_class.add_function(self.getDecodeFunction(relevant_body_elements_numbered))


    def getDecodeFunction(self, elements) -> CppFunction:
        code = f"uint8_t message_type;\n" \
               f"bit_stream_->get_next_n_bits(6, &message_type);\n" \
               f"uint8_t one_bit;" \
               f"bit_stream_->get_next_n_bits(1, &one_bit);\n" \
               f"switch(message_type) ""{\n"

        for key, value in elements.items():
            if value.type.local_name in self.complex_type_names:
                code += f"\tcase {key}:\n" \
                        f"\t\tstring_stream_->start_key(\"{value.local_name}\");\n" \
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
                f"string_stream_->end_key();\n"

        return CppFunction(function_name="decodeBody",
                           return_type="void",
                           arguments="void",
                           code=code,
                           comment=None)

    def write_body_conversion_header(self, directory: str):
        self.cpp_class.write_h(directory)

    def write_body_conversion_source(self, directory: str):
        self.cpp_class.write_cpp(directory)
