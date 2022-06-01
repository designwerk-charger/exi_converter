from typing import List

from cpp.cpp_function import CppFunction
from datatypes.base_type import BaseType
from datatypes.element import Element


class ComplexType(BaseType):
    def __init__(self, name, namespace, base_class: str, is_abstract: bool):
        super().__init__(name, namespace)
        self.is_simple_not_complex = False
        self.is_abstract = is_abstract
        self.child_elements: List[Element] = []
        self.base_class = base_class
        self.derived_classes = []

    def add_derived_class(self, derived_class: BaseType):
        self.derived_classes.append(derived_class)

    def __str__(self):
        absract_str = "abstract; " if self.is_abstract else " "
        header_line = f"{self.type_name}; complex;{absract_str}{self.base_class}; {self.type_namespace}\n"
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
        for derived in self.derived_classes:
            for derived_child in derived.get_child_types():
                childs.append(derived_child)
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
                           return_type="bool", arguments=None,
                           comment="",
                           code="")
