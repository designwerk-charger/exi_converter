from typing import List

from cpp.cpp_function import CppFunction
from datatypes.base_type import BaseType
from datatypes.element import Element


class ComplexType(BaseType):
    def __init__(self, name, namespace, child_elements: List[Element]):
        super().__init__(name, namespace)
        self.is_simple_not_complex = False
        self.child_elements = child_elements

    def __str__(self):
        header_line = f"{self.type_name}; complex; {self.type_namespace}\n"
        child_lines = f""
        for child in self.child_elements:
            child_txt = str(child)
            for line in child_txt.splitlines():
                child_lines += f"\t{line}\n"
        return header_line + child_lines

    def get_simple_types(self):
        all_simple_types = {}
        for child in self.child_elements:
            all_simple_types.update(child.element_type.get_simple_types())
        return all_simple_types

    def get_child_types(self) -> List[BaseType]:
        childs = [self]
        for child in self.child_elements:
            for child_child in child.element_type.get_child_types():
                childs.append(child_child)
        return childs

    @property
    def encode_function(self):
        return CppFunction(function_name=f"encode_{self.type_name}",
                           return_type="void", arguments="bool",
                           comment="",
                           code="")

    @property
    def decode_function(self):
        return CppFunction(function_name=f"decode_{self.type_name}",
                           return_type="bool", arguments="void",
                           comment="",
                           code="")
