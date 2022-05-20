from typing import Optional


class CppFunction:

    def __init__(self, function_name: str, return_type: str, arguments: str, code: str, comment: Optional[str]):
        self.function_name = function_name
        self.return_type = return_type
        self.arguments = arguments
        self.code = code
        self.comment = comment

    def get_definition(self, indentation=1, virtual_function=False):
        indent_str = "\t" * indentation
        comment = f"{indent_str}// {self.comment}\n" if self.comment else ""
        virtual = "virtual " if virtual_function else ""
        return f"{comment}" \
               f"{indent_str}{virtual}{self.return_type} {self.function_name}({self.arguments});"

    def get_implementation(self, class_name: str):
        function_str = f"{self.return_type} {class_name}::{self.function_name}({self.arguments}) ""{\n"
        for l in self.code.splitlines():
            function_str += f"\t{l}\n"
        return function_str + "}"


