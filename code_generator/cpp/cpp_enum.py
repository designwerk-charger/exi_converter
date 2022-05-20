

class CppEnum:

    def __init__(self, enum_name: str):
        self.enum_name = enum_name
        self.items = {}

    def add_items(self, item: str):
        self.items[item] = len(self.items)

    def get_definition(self):
        if len(self.items) == 0:
            raise RuntimeError("Can not generate enum without items!")

        cpp_code = f"enum class {self.enum_name} ""{\n"
        for key in self.items.keys():
            cpp_code += f"\t{key} = {self.items[key]},""\n"
        cpp_code += "};"
        return cpp_code
