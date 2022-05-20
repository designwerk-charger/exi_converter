import abc
from typing import List

from cpp.cpp_function import CppFunction
from datatypes.base_type import BaseType


class SimpleType(BaseType):

    def __init__(self, type_name: str, type_namespace: str):
        super().__init__(type_name, type_namespace)
        self.is_simple_not_complex = True

    def __str__(self):
        return f"simple; {self.type_name}; {self.type_namespace}"

    @abc.abstractmethod
    def get_base_type_functions(self):
        raise NotImplementedError

    def get_simple_types(self):
        return {str(self.__class__.__name__): self}

    def get_child_types(self):
        return [self]


class BoolType(SimpleType):

    def __str__(self):
        return f"BoolType; {self.type_namespace}"

    def get_base_type_functions(self) -> List[CppFunction]:
        functions = []
        functions.append(CppFunction(function_name="extractBoolValue",
                                     return_type="bool", arguments="void",
                                     comment="/ \\brief read a bit from bitstream and returns the interpreted value",
                                     code=""))
        functions.append(CppFunction(function_name="injectBoolValue",
                                     return_type="void", arguments="bool",
                                     comment="/ \\brief writes the given boolean value to the bitstream",
                                     code=""))
        return functions

    def generate_code(self):
        return ""


class Base64Type(SimpleType):

    def __str__(self):
        return f"Base64Type; {self.type_namespace}"

    def get_base_type_functions(self) -> List[CppFunction]:
        functions = []
        functions.append(CppFunction(function_name="extractBase64Value",
                                     return_type="std::string", arguments="void",
                                     comment="",
                                     code=""))
        return functions


class EnumType(SimpleType):

    def __init__(self, type_name: str, type_namespace: str, enumerations: List[str]):
        super().__init__(type_name, type_namespace)
        self.enumerations = enumerations

    def __str__(self):
        return f"EnumType; {self.type_namespace}"

    def get_base_type_functions(self) -> List[CppFunction]:
        functions = []
        functions.append(CppFunction(function_name="extractNBitsForEnum",
                                     return_type="uint32_t", arguments="uint32_t n_bits",
                                     comment="",
                                     code=""))
        return functions


class StringType(SimpleType):

    def __str__(self):
        return f"StringType; {self.type_namespace}"

    def get_base_type_functions(self) -> List[CppFunction]:
        functions = []
        functions.append(CppFunction(function_name="extractString",
                                     return_type="std::string", arguments="void",
                                     comment="",
                                     code=""))
        return functions


class DecimalType(SimpleType):

    def __str__(self):
        return f"DecimalType; {self.type_namespace}"

    def get_base_type_functions(self) -> List[CppFunction]:
        functions = []
        functions.append(CppFunction(function_name="extractNaturalNumber",
                                     return_type="uint32_t", arguments="uint8_t n_bytes",
                                     comment="",
                                     code=""))
        functions.append(CppFunction(function_name="extractIntegerNumber",
                                     return_type="int32_t", arguments="uint8_t n_bytes",
                                     comment="",
                                     code=""))
        return functions


class HexBinType(SimpleType):

    def __str__(self):
        return f"HexBinType; {self.type_namespace}"

    def get_base_type_functions(self) -> List[CppFunction]:
        functions = []
        functions.append(CppFunction(function_name="extractHexBin",
                                     return_type="std::string", arguments="uint8_t bytes",
                                     comment="",
                                     code=""))
        return functions
