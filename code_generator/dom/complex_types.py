from typing import List

from cpp.cpp_class import CppClass
from cpp.cpp_function import CppFunction
from datatypes.base_type import BaseType
from datatypes.complex_type import ComplexType
from datatypes.element import Element


class ComplexTypes:

    def __init__(self, type_tree: List[BaseType]):
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

    def getDecodeCodeForElement(self, element: Element) -> str:
        def decode_simple_element(indent) -> str:
            indent_str = "\t" * indent
            return f"{indent_str}// decode simple type\n" \
                   f"{indent_str}auto {element.element_name} = {element.element_type.decode_function.call()};\n" \
                   f"{indent_str}string_stream_->start_key(\"{element.element_name}\");\n" \
                   f"{indent_str}string_stream_->add_value({element.element_name});\n" \
                   f"{indent_str}string_stream_->end_key();\n"

        def decode_complex_element(indent) -> str:
            indent_str = "\t" * indent
            return f"{indent_str}// decode complex type\n" \
                   f"{indent_str}string_stream_->start_key(\"{element.element_name}\");\n" \
                   f"{indent_str}{element.element_type.decode_function.call()};\n" \
                   f"{indent_str}string_stream_->end_key();\n"

        def decode_element(indent) -> str:
            if element.element_type.is_simple_not_complex:
                return decode_simple_element(indent)
            return decode_complex_element(indent)

        def check_if_optional() -> str:
            if element.is_optional:
                code = f"bool {element.element_name}_available = base_types_->extractBoolValue();\n" \
                       f"if({element.element_name}_available) ""{\n"
                code += decode_element(indent=1)
                return code + "\n}\n"
            return decode_element(indent=0)

        return check_if_optional()

    def getDecodeFunction(self, ct: ComplexType):
        code = ""
        for element in ct.child_elements:
            code += self.getDecodeCodeForElement(element)
            code += "\n\n"

        return CppFunction(function_name=ct.decode_function.function_name,
                           return_type="void",
                           arguments="void",
                           code=code,
                           comment=None)

    def write_complex_type_conversion_header(self, directory: str):
        self.cpp_class.write_h(directory)

    def write_complex_type_conversion_source(self, directory: str):
        self.cpp_class.write_cpp(directory)
