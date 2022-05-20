from datatypes.base_type import BaseType


class Element:

    def __init__(self, element_name: str, element_type: BaseType, is_optional: bool, max_items=1):
        self.element_name = element_name
        self.element_type = element_type
        self.is_optional = is_optional
        self._max_items = max_items
        self.is_list = max_items != 1

    def __str__(self):
        txt = ""
        if self.is_list:
            txt += "["
        if self.is_optional:
            txt += f"({self.element_name})"
        else:
            txt += f"{self.element_name}"
        if self.is_list:
            txt += f", {self._max_items}]"

        return f"{txt}: {self.element_type}"
