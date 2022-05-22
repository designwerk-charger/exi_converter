from typing import List

from cpp.cpp_class import CppClass
from datatypes.base_type import BaseType


class BaseTypes:

    def __init__(self, type_tree: List[BaseType]):
        self.simple_types = {}
        for item in type_tree:
            self.simple_types.update(item.get_simple_types())

        self.cpp_class = CppClass(class_name="BaseTypes", derived_from_class=None, virtual=True,
                                  includes="#include <cstdint>\n#include <cstdio>\n#include <string>\n"
                                           "#include \"base/bitstream.h\"\n")

        for t in self.simple_types.values():
            self.cpp_class.add_functions(t.get_base_type_functions())

    def write_base_type_header(self, directory: str):
        self.cpp_class.write_h(directory)
        # self.cpp_class.write_cpp(directory)
