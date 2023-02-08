import os
from math import log2, ceil
from typing import List

from cpp.cpp_class import CppClass
from cpp.cpp_function import CppFunction
from datatypes.base_type import BaseType
from datatypes.simple_type import EnumType


class Enums:

    def __init__(self, type_tree: List[BaseType], namespace=None):
        all_types = {}
        for item in type_tree:
            for type in item.get_child_types():
                all_types[type.type_name] = type

        self.all_enum_types = []
        for t in all_types.values():
            if isinstance(t, EnumType):
                self.all_enum_types.append(t)

        self.cpp_class = CppClass(class_name="EnumTypes", derived_from_class=None,
                                  includes="#include <cstdint>\n#include <cstdio>\n#include <string>\n#include <sstream>\n#include <unordered_map>\n"
                                           "#include \"type_conversion/base_types.h\"\n", namespace=namespace)
        self.cpp_class.add_member("BaseTypes * base_types;")
        self.cpp_class.add_constructor("BaseTypes * base_types",
                                       "this->base_types = base_types;")

        for enum in self.all_enum_types:
            self.cpp_class.add_function(self.getDecodeFunction(enum))
            self.cpp_class.add_function(self.getEncodeFunction(enum))

    def getDecodeFunction(self, enum: EnumType):
        num_items = len(enum.enumerations)
        num_bits = ceil(log2(num_items))
        code = "const std::string ENUM2STR[] = ""{\n"
        for item in enum.enumerations:
            code += f"\t\"{item}\",\n"
        code += "};\n"
        code += f"uint8_t enum_num = base_types->extractNBitsForEnum({num_bits});\n" \
                f"if (enum_num >= {num_items}) [[unlikely]] ""{\n" \
                "\tstd::ostringstream stm;\n" \
                "\tstm << \"Enum value \" << enum_num << \"exceeds maximum of {num_items}!\";\n" \
                f"\tthrow std::runtime_error(stm.str());\n" \
                f"}}\n" \
                f"return ENUM2STR[enum_num];\n"
        return CppFunction(function_name=f"decode_{enum.type_name}",
                    return_type="std::string",
                    arguments=None,
                    code=code,
                    comment=None)

    def getEncodeFunction(self, enum: EnumType):
        num_items = len(enum.enumerations)
        num_bits = ceil(log2(num_items))
        code = f"const uint8_t n_bits = {num_bits};\n" \
               f"static std::unordered_map<std::string, uint8_t> const table = {{\n"
        for i, item in enumerate(enum.enumerations):
            code += f"\t{{\"{item}\", {i}}},\n"
        code += f"}};\n" \
                f"auto it = table.find(str_enum);\n" \
                f"if (it != table.end()) {{\n" \
                "\tauto enum_value = it->second;\n" \
                "\tbase_types->injectNBitsForEnum(enum_value, n_bits);\n" \
                f"}} else {{\n" \
                f"\tstd::ostringstream stm;\n" \
                f"\tstm << \"The Enum value \" << str_enum << \" is not part of the Enum {enum.type_name}\";\n" \
                f"\tthrow std::runtime_error(stm.str());\n" \
                f"}}\n"
        return CppFunction(function_name=f"encode_{enum.type_name}",
                           return_type="void",
                           arguments="const std::string & str_enum",
                           code=code,
                           call_args="value",
                           comment=None)

    def write_enum_conversion_header(self, directory: str):
        self.cpp_class.write_h(directory)

    def write_enum_conversion_source(self, directory: str):
        self.cpp_class.write_cpp(directory)
