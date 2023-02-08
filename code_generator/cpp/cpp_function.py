from typing import Optional


class CppFunction:

    def __init__(self, function_name: str, return_type: str, arguments: Optional[str], code: str, comment: Optional[str], member_object_name="", call_args=""):
        self.function_name = function_name
        self.return_type = return_type
        self.arguments = arguments
        self.code = code
        self.comment = comment
        self.member_object_name = member_object_name
        self.call_args = call_args

    def call(self, call_args: Optional[str] = None):
        if call_args is None:
            call_args = self.call_args
        member_call_str = ""
        if self.member_object_name:
            member_call_str = f"{self.member_object_name}->"
        return f"{member_call_str}{self.function_name}({call_args})"

    def get_definition(self, indentation=1, virtual_function=False):
        indent_str = "\t" * indentation
        comment = f"{indent_str}// {self.comment}\n" if self.comment else ""
        virtual = "virtual " if virtual_function else ""
        arguments = self.arguments if self.arguments else ""
        return f"{comment}" \
               f"{indent_str}{virtual}{self.return_type} {self.function_name}({arguments});"

    def get_implementation(self, class_name: str):
        arguments = self.arguments if self.arguments else ""
        function_str = f"{self.return_type} {class_name}::{self.function_name}({arguments}) ""{\n"
        for l in self.code.splitlines():
            function_str += f"\t{l}\n"
        return function_str + "}"


