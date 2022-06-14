from typing import Dict

from datatypes.base_type import BaseType


class Element:

    def __init__(self, element_name: str, element_type: BaseType, is_optional: bool, max_items: int, substitutes: Dict):
        self.element_name = element_name
        self.element_type = element_type
        self.is_optional = is_optional
        self.max_items = max_items
        self.is_list = False
        if not isinstance(max_items, int):
            print(f"the max item ({max_items}) for element {self.element_name} of type {self.element_type.type_name} is not a number. Assuming 40 as maximum!")
            self.max_items = 40
            self.is_list = True
        elif max_items > 1:
            self.is_list = True
        self.substitutes = substitutes

    def __str__(self):
        txt = self.__class__.__name__
        if self.is_list:
            txt += "["
        if self.is_optional:
            txt += f" ({self.element_name})"
        else:
            txt += f" {self.element_name}"
        if self.is_list:
            txt += f", {self.max_items}]"

        return f"{txt}: {self.element_type}"


class AnyElement(Element):

    def __init__(self, element_name: str, element_type: BaseType, is_optional: bool):
        super().__init__(element_name, element_type, is_optional, max_items=1, substitutes={})


class Attribute(Element):

    def __init__(self, element_name: str, element_type: BaseType, is_optional: bool):
        super().__init__(element_name, element_type, is_optional, max_items=1, substitutes={})
