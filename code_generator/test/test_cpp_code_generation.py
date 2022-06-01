import tempfile
import unittest

from cpp.cpp_class import CppClass
from cpp.cpp_enum import CppEnum
from cpp.cpp_function import CppFunction


class TestCppClassCodeGeneration(unittest.TestCase):

    def setUp(self) -> None:
        self.cpp_class = CppClass(class_name="TestClass", derived_from_class=None, includes="")
        self.cpp_class.add_function(CppFunction(function_name="doNothing",
                                                return_type="void",
                                                arguments=None,
                                                code=""))
        self.cpp_class.add_function(CppFunction(function_name="returnTrueWhenInput1",
                                                return_type="bool",
                                                arguments="int input",
                                                code="return input==1;"))
        self.tempdir = tempfile.TemporaryDirectory(dir="/tmp/")

    def tearDown(self) -> None:
        self.tempdir.cleanup()

    def test_get_definition(self):
        class_impl = self.cpp_class.get_definition_file()
        print(class_impl)

    def test_get_implementation(self):
        class_def = self.cpp_class.get_implementation()
        print(class_def)

    def test_write_header(self):
        self.cpp_class.write_h(self.tempdir.name)

    def test_write_cpp(self):
        self.cpp_class.write_cpp(self.tempdir.name)


class TestCppEnumCodeGeneration(unittest.TestCase):

    def test_get_definition(self):
        enum = CppEnum(enum_name="TestEnum")
        enum.add_items("red")
        enum.add_items("blue")
        print(enum.get_definition())
