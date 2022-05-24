from math import ceil, log2
from typing import List

from cpp.cpp_class import CppClass
from cpp.cpp_function import CppFunction
from datatypes.base_type import BaseType
from datatypes.complex_type import ComplexType
from datatypes.element import Element


class ComplexTypes:

    def __init__(self, type_tree: List[BaseType]):
        self.optional_parameter_suffix_cnt = 0
        all_types = {}
        for item in type_tree:
            for type in item.get_child_types():
                all_types[type.type_name] = type

        self.all_complex_types = []
        for t in all_types.values():
            if isinstance(t, ComplexType):
                self.all_complex_types.append(t)

        self.cpp_class = CppClass(class_name="ComplexTypes", derived_from_class=None,
                                  includes="#include <cstdint>\n#include <cstdio>\n#include <string>\n#include <sstream>\n#include <unordered_map>\n"
                                           "#include \"base_types.h\"\n#include \"enum_types.h\"\n#include \"base/stringstream.h\"\n")
        self.cpp_class.add_member("BaseTypes * base_types_;\n\tEnumTypes * enum_types_;\n\tStringStream * string_stream_;")
        self.cpp_class.add_constructor("BaseTypes * base_types, EnumTypes * enum_types, StringStream * string_stream",
                                       "base_types_ = base_types;\nenum_types_ = enum_types;\nstring_stream_ = string_stream;\n")

        for ct in self.all_complex_types:
            self.cpp_class.add_function(self.getDecodeFunction(ct))

    @staticmethod
    def _decode_simple_element(element: Element, indent: int) -> str:
        indent_str = "\t" * indent
        return f"{indent_str}string_stream_->start_key(\"{element.element_name}\");\n" \
               f"{indent_str}string_stream_->add_value({element.element_type.decode_function.call()});\n" \
               f"{indent_str}string_stream_->end_key();\n"
              # f"{indent_str}auto {element.element_name} = {element.element_type.decode_function.call()};\n" \
              # f"{indent_str}string_stream_->add_value({element.element_name});\n" \


    @staticmethod
    def _decode_simple_element_with_event_code(element: Element, indent: int) -> str:
        indent_str = "\t" * indent
        return f"{indent_str}// decode simple type\n" \
               f"{indent_str}base_types_->check_event_code_is_0(\"Start{element.element_name}\");\n" \
               f"{ComplexTypes._decode_simple_element(element, indent)}" \
               f"{indent_str}base_types_->check_event_code_is_0(\"End{element.element_name}\");\n"

    @staticmethod
    def _decode_complex_element(element: Element, indent: int) -> str:
        indent_str = "\t" * indent
        return f"{indent_str}string_stream_->start_key(\"{element.element_name}\");\n" \
               f"{indent_str}{element.element_type.decode_function.call()};\n" \
               f"{indent_str}string_stream_->end_key();\n"

    @staticmethod
    def _decode_complex_element_with_event_code(element: Element, indent: int) -> str:
        indent_str = "\t" * indent
        return f"{indent_str}// decode complex type\n" \
               f"{indent_str}base_types_->check_event_code_is_0(\"Start{element.element_name}\");\n" \
               f"{ComplexTypes._decode_complex_element(element, indent)}" \
               f"{indent_str}base_types_->check_event_code_is_0(\"End{element.element_name}\");\n"

    @staticmethod
    def decode_element(element: Element, indent: int) -> str:
        if element.element_type.is_simple_not_complex:
            return ComplexTypes._decode_simple_element(element, indent)
        return ComplexTypes._decode_complex_element(element, indent)

    @staticmethod
    def decode_element_with_event_code(element: Element, indent: int) -> str:
        if element.element_type.is_simple_not_complex:
            return ComplexTypes._decode_simple_element_with_event_code(element, indent)
        return ComplexTypes._decode_complex_element_with_event_code(element, indent)

    def getDecodeCodeForOptionalBlob(self, elements: List[Element], indent: int) -> str:
        self.optional_parameter_suffix_cnt += 1
        indent_str = "\t" * indent
        eventcode_bits = ceil(log2(len(elements)+1)) # +1 because one optional parameter gets at least 2 bits reading
        names = "_".join([e.element_name for e in elements])
        code = f"{indent_str}uint8_t ec{self.optional_parameter_suffix_cnt} = base_types_->get_event_code_with_n_bits({eventcode_bits}, \"Start{names}\");\n"
        code += f"{indent_str}switch(ec{self.optional_parameter_suffix_cnt}) {{\n"
        for i, element in enumerate(elements):
            code += f"{indent_str}\tcase {i}:\n"
            code += f"{indent_str}{ComplexTypes.decode_element(element, indent+2)}"
            code += f"{indent_str}\t\tbreak;\n"
        if elements[-1].is_optional:  # special case for optional messages at the end of a complex type
            code += f"{indent_str}\tcase {len(elements)}: break;\n"
        code += f"{indent_str}\tdefault:\n" \
                f"{indent_str}\t\tthrow std::runtime_error(\"While parsing event code for '{names}' a unexpected code (\" +  std::to_string(ec{self.optional_parameter_suffix_cnt}) + \") appeared  0!\");\n" \
                f"{indent_str}}}"
        return code

    def getDecodeFunction(self, ct: ComplexType):
        code = ""
        optional_blob = []
        for element in ct.child_elements:
            if element.is_optional:
                optional_blob.append(element)
            elif len(optional_blob) != 0:
                optional_blob.append(element)
                code += self.getDecodeCodeForOptionalBlob(optional_blob, 0)
                code += "\n"
                optional_blob = []
            else:
                code += self.decode_element_with_event_code(element, 0)
                code += "\n"
        if len(optional_blob) != 0:
            code += self.getDecodeCodeForOptionalBlob(optional_blob, 0)

        return CppFunction(function_name=ct.decode_function.function_name,
                           return_type="void",
                           arguments="void",
                           code=code,
                           comment=None)

    def write_complex_type_conversion_header(self, directory: str):
        self.cpp_class.write_h(directory)

    def write_complex_type_conversion_source(self, directory: str):
        self.cpp_class.write_cpp(directory)
