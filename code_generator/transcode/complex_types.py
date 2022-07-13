from math import ceil, log2
from typing import List

from cpp.cpp_class import CppClass
from cpp.cpp_function import CppFunction
from datatypes.base_type import BaseType
from datatypes.complex_type import ComplexType
from datatypes.element import Element, Attribute, AnyElement


class ComplexTypes:

    def __init__(self, type_tree: List[BaseType], namespace=None):
        self._local_suffix_cnt = 0
        self.all_types = {}
        for item in type_tree:
            for type in item.get_child_types():
                self.all_types[type.base_type_name] = type

        self.all_complex_types = []
        for t in self.all_types.values():
            if isinstance(t, ComplexType):
                self.all_complex_types.append(t)

        self.cpp_class = CppClass(class_name="ComplexTypes", derived_from_class=None,
                                  includes="#include <iostream>\n#include <cstdint>\n#include <cstdio>\n"
                                           "#include <string>\n#include <sstream>\n#include <unordered_map>\n"
                                           "#include \"type_conversion/base_types.h\"\n#include \"enum_types.h\"\n"
                                           "#include \"base/output_string_stream.h\"\n"
                                           "#include \"base/input_string_stream.h\"\n",
                                  namespace=namespace)
        self.cpp_class.add_member("BaseTypes * base_types_;\n\tEnumTypes * enum_types_;\n"
                                  "\tOutputStringStream * output_string_stream_;\n"
                                  "\tInputStringStream * input_string_stream_;\n"
                                  "\tenum { arrsize = 16 };\n"
                                  "\tstatic constexpr uint8_t log_lookup[arrsize] = {1, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4};\n")
        self.cpp_class.add_constructor("BaseTypes * base_types, EnumTypes * enum_types, OutputStringStream * output_string_stream",
                                       "base_types_ = base_types;\nenum_types_ = enum_types;\n"
                                       "output_string_stream_ = output_string_stream;\ninput_string_stream_ = nullptr;\n")
        self.cpp_class.add_constructor("BaseTypes * base_types, EnumTypes * enum_types, InputStringStream * input_string_stream",
                                       "base_types_ = base_types;\nenum_types_ = enum_types;\n"
                                       "input_string_stream_ = input_string_stream;\noutput_string_stream_ = nullptr;\n")

        for ct in self.all_complex_types:
            self.cpp_class.add_function(self.getDecodeFunction(ct))
            self.cpp_class.add_function(self.getEncodeFunction(ct))

    @property
    def local_suffix_cnt(self):
        self._local_suffix_cnt += 1
        return self._local_suffix_cnt

################################################################################
# Simple helpers
################################################################################

    @staticmethod
    def sort_optional_and_abstract_elements(elements: List[Element]) -> str:
        anyE = []
        E = []
        A = []
        for e in elements:
            if isinstance(e, AnyElement):
                anyE.append(e)
            elif isinstance(e, Attribute):
                A.append(e)
            elif isinstance(e, Element):
                E.append(e)
            else:
                raise RuntimeError("Unexpected type")
        return A + E + anyE

    @staticmethod
    def get_all_elements_flat(elements: List[Element]):
        all_elements_flat = []
        for e in elements:
            if e.element_type.is_abstract:
                for (element_name, element_type) in e.substitutes.items():
                    all_elements_flat.append(Element(element_name=element_name, element_type=element_type, is_optional=e.is_optional, max_items=1, substitutes={}))
            else:
                all_elements_flat.append(e)
        return all_elements_flat

    @staticmethod
    def get_num_AnyElements(elements: List[Element]) -> int:
        return sum([isinstance(e, AnyElement) for e in elements])

    @staticmethod
    def get_num_abstract_elements_per_base(elem: Element):
        if elem.element_type.is_simple_not_complex or elem.element_type.base_class is None:
            return 1
        return len(elem.element_type.base_class.derived_classes) + 1

    @staticmethod
    def do_abstract_element_checks(element: Element):
        if not isinstance(element.element_type, ComplexType):
            raise NotImplementedError(f"The type of Element {element.element_name} is not ComplexType")
        if len(element.element_type.derived_classes) == 0:
            print(f"WARNING: abstract class {element.element_type.type_name} has no derived classes")
        if len(element.element_type.derived_classes) != len(element.substitutes):
            print(f"WARNING: abstract class {element.element_type.type_name} has not same number of derivations as the element has substitutes")
        if element.is_optional:
            print(f"WARNING: element {element.element_name} with abstract class {element.element_type.type_name} is optional!")

    @staticmethod
    def get_num_eventcode_bits(elements: List[Element], is_last: bool, progress=0) -> int:
        cnt = 1  # +1 because one optional parameter gets at least 2 bits reading
        cnt += len(ComplexTypes.get_all_elements_flat(elements)) + ComplexTypes.get_num_AnyElements(elements)
        cnt -= progress

        if is_last:
            cnt += 1
        return ceil(log2(cnt))

################################################################################
# DECODE
################################################################################

    @staticmethod
    def _decode_simple_type(element: Element, indent: int, from_optional=False) -> str:
        indent_str = "\t" * indent
        return_str = ""
        if from_optional:
            return_str += f"{indent_str}//   without check_event_code_is_0(...) because previous element was optional;\n"
        else:
            return_str += f"{indent_str}base_types_->check_event_code_is_0(\"Start+{element.element_name}\");\n"
        return_str += \
               f"{indent_str}output_string_stream_->start_key(\"{element.element_name}\");\n" \
               f"{indent_str}auto var = {element.element_type.decode_function.call()};\n" \
               f"{indent_str}output_string_stream_->add_value(var);\n" \
               f"{indent_str}#ifndef NDEBUG\n" \
               f"{indent_str}\tstd::cout << \"getting value for {element.element_type.type_name} '{element.element_name}' -> \" << var << std::endl;\n" \
               f"{indent_str}#endif\n" \
               f"{indent_str}output_string_stream_->end_key();\n"
        return return_str

    @staticmethod
    def _decode_simple_element(element: Element, indent: int, from_optional=False) -> str:
        indent_str = "\t" * indent
        optional_str = "optional " if element.is_optional else ""
        if isinstance(element, Attribute):
            return f"{indent_str}{{  // decode {optional_str}simple Attribute type {element.element_type.type_name}\n" \
                   f"{ComplexTypes._decode_simple_type(element, indent + 1, from_optional)}" \
                   f"{indent_str}}}\n"
        return f"{indent_str}{{  // decode {optional_str}simple Element type {element.element_type.type_name}\n" \
               f"{indent_str}\tbase_types_->check_event_code_is_0(\"Start{element.element_name}\");\n" \
               f"{ComplexTypes._decode_simple_type(element, indent + 1, from_optional)}" \
               f"{indent_str}\tbase_types_->check_event_code_is_0(\"End{element.element_name}\");\n" \
               f"{indent_str}}}\n"

    def _decode_complex_element(self, element: Element, indent: int, from_optional=False) -> str:
        indent_str = "\t" * indent
        optional_str = "optional " if element.is_optional else ""

        return_str = f"{indent_str}// decode {optional_str}complex element {element.element_name} with type {element.element_type.type_name}\n"
        if from_optional:
            return_str += f"{indent_str}//   without check_event_code_is_0(...) because previous element was optional;\n"
        else:
            return_str += f"{indent_str}base_types_->check_event_code_is_0(\"Start{element.element_name}\");\n"
        return_str += f"{indent_str}output_string_stream_->start_key(\"{element.element_name}\");\n" \
                      f"{indent_str}{element.element_type.decode_function.call()};\n" \
                      f"{indent_str}output_string_stream_->end_key();\n"
        return return_str

    def decode_element(self, element: Element, indent: int, from_optional=False) -> str:
        if element.element_type.is_simple_not_complex:
            return ComplexTypes._decode_simple_element(element, indent, from_optional)
        return self._decode_complex_element(element, indent, from_optional)

    def decode_list(self, element: Element, indent: int) -> str:
        indent_str = "\t" * indent
        suffix = self.local_suffix_cnt
        if element.is_optional:
            code = ""
        else:
            code = f"{indent_str}base_types_->check_event_code_is_0(\"StartList{element.element_name}\");\n"

        code += f"{indent_str}output_string_stream_->start_key(\"{element.element_name}\");\n" \
                f"{indent_str}output_string_stream_->start_list();\n"

        code += f"{indent_str}int further_items{suffix};\n" \
                f"{indent_str}for(int i=0; i<{element.max_items}; i++) {{;\n" \
                f"{indent_str}\tif (i!=0) {{\n" \
                f"{indent_str}\t\tfurther_items{suffix} = base_types_->get_event_code_with_n_bits(2, \"ListItem{element.element_name}\");\n" \
                f"{indent_str}\t\tif (further_items{suffix} == 1) {{\n" \
                f"{indent_str}\t\t\tbreak;\n" \
                f"{indent_str}\t\t}} else if (further_items{suffix} == 0) {{\n" \
                f"{indent_str}\t\t\toutput_string_stream_->next_item();\n" \
                f"{indent_str}\t\t}} else {{\n" \
                f"{indent_str}\t\t\tthrow std::runtime_error(\"Unexpected code while decoding {element.element_name} List!\");\n" \
                f"{indent_str}\t\t}}\n" \
                f"{indent_str}\t}}\n"

        if element.element_type.is_simple_not_complex:
            code += f"{indent_str}\t{{\n" \
                    f"{indent_str}\t\tbase_types_->check_event_code_is_0(\"Start{element.element_name}\");\n" \
                    f"{indent_str}\t\tauto var = {element.element_type.decode_function.call()};\n" \
                    f"{indent_str}\t\toutput_string_stream_->add_value(var);\n" \
                    f"{indent_str}\t\t#ifndef NDEBUG\n" \
                    f"{indent_str}\t\t\tstd::cout << \"getting value for {element.__class__.__name__} '{element.element_name}' -> \" << var << std::endl;\n" \
                    f"{indent_str}\t\t#endif\n"
            code += f"{indent_str}\t\tbase_types_->check_event_code_is_0(\"End{element.element_name}\");\n"
            code += f"{indent_str}\t}}\n"
        else:
            code += f"{indent_str}{element.element_type.decode_function.call()};\n"

        code += f"{indent_str}}}\n" \
                f"{indent_str}output_string_stream_->end_list();\n" \
                f"{indent_str}output_string_stream_->end_key();\n"

        return code

    def get_decode_code_for_optional_and_abstract_blob(self, elements: List[Element], is_last: bool) -> str:
        suffix = self.local_suffix_cnt
        while_loop_offset = 0
        all_elements_flat = ComplexTypes.get_all_elements_flat(elements)
        names = "_".join([e.element_name for e in all_elements_flat])
        end_str = " + END_ELEMENT" if is_last else ""
        code = f"// Decoding optional elements: {[en.element_name for en in all_elements_flat]}{end_str}\n" \
               f"uint8_t ec{suffix} = base_types_->get_event_code_with_n_bits({max(ComplexTypes.get_num_eventcode_bits(elements, is_last), 2)}, \"Start{names}\");\n" \
               f"bool continue_loop{suffix} = true;\n" \
               f"while(continue_loop{suffix}){{\n" \
               f"switch(ec{suffix}) {{\n"

        for i, element in enumerate(all_elements_flat):
            code += f"\tcase {i}:\n"
            if element.is_list:
                code += self.decode_list(element, 2)
            else:
                code += f"{self.decode_element(element, 2, from_optional=True)}"
            while_loop_offset += ComplexTypes.get_num_abstract_elements_per_base(element)
            if element.is_optional and ((i != (len(all_elements_flat)-1)) or is_last):
                # not last element
                code += f"\t\tec{suffix} = {while_loop_offset} + base_types_->get_event_code_with_n_bits(" \
                        f"{max(1, ComplexTypes.get_num_eventcode_bits(elements, is_last, while_loop_offset))}, \"Start{names}{i}\");\n"
            else:
                code += f"\t\tcontinue_loop{suffix} = false;\n"
            code += f"\t\tbreak;\n"

        if all_elements_flat[-1].is_optional:  # special case for optional messages at the end of a complex type
            code += f"\tcase {len(all_elements_flat)}:\n" \
                    f"\t\tcontinue_loop{suffix} = false;\n" \
                    f"\t\tbreak;\n"
        code += f"\tdefault:\n" \
                f"\t\tthrow std::runtime_error(\"While parsing event code for '{names}' a unexpected code (\" +  std::to_string(ec{suffix}) + \") appeared  0!\");\n" \
                f"}}\n"
        code += f"}}"
        return code

    def getDecodeFunction(self, ct: ComplexType):
        code = ""
        optional_blob = []
        was_normal_complex = False
        for element in ct.child_elements:
            was_normal_complex = False
            if element.is_list:
                if element.is_optional:
                    print(f"WARNING: list as optional types not yet tested ({element.element_name})")
                    optional_blob.append(element)
                elif len(optional_blob) != 0:
                    # finish optional list and decode list
                    sorted_blob = ComplexTypes.sort_optional_and_abstract_elements(optional_blob)
                    code += self.get_decode_code_for_optional_and_abstract_blob(sorted_blob, is_last=False)
                    code += "\n"
                    code += self.decode_list(element, 0)
                    optional_blob = []
                else:
                    code += self.decode_list(element, 0)
            elif element.element_type.is_abstract and not element.is_optional:
                self.do_abstract_element_checks(element)
                optional_blob.append(element)
                # finish optional list with abstract types
                sorted_blob = ComplexTypes.sort_optional_and_abstract_elements(optional_blob)
                code += self.get_decode_code_for_optional_and_abstract_blob(sorted_blob, is_last=False)
                code += "\n"
                optional_blob = []
            elif element.is_optional:
                optional_blob.append(element)
            elif len(optional_blob) != 0:
                # finish optional list and add new element
                sorted_blob = ComplexTypes.sort_optional_and_abstract_elements(optional_blob)
                code += self.get_decode_code_for_optional_and_abstract_blob(sorted_blob, is_last=True)
                code += "\n"
                code += self.decode_element(element, 0, from_optional=optional_blob[-1].is_optional)
                optional_blob = []
                code += "\n"
                was_normal_complex = True
            else:
                code += self.decode_element(element, 0)
                code += "\n"
                was_normal_complex = True

        if len(optional_blob) != 0:
            sorted_blob = ComplexTypes.sort_optional_and_abstract_elements(optional_blob)
            code += self.get_decode_code_for_optional_and_abstract_blob(sorted_blob, is_last=True)

        if was_normal_complex:
            # Add Finish reading bit when type not ending with a list ore abstract element
            code += f"base_types_->check_event_code_is_0(\"Finish{ct.type_name}\");\n"

        return CppFunction(function_name=ct.decode_function.function_name,
                           return_type="void",
                           arguments=None,
                           code=code,
                           comment=None)

################################################################################
# ENCODE
################################################################################

    def _encode_simple_type(self, element: Element, indent: int, from_optional=False) -> str:
        indent_str = "\t" * indent
        code = ""

        if from_optional:
            code += f"{indent_str}//   without add_event_code(...) because previous element was optional;\n"
        else:
            code += f"{indent_str}base_types_->add_event_code(\"Start+{element.element_name}\");\n"

        code += f"{indent_str}input_string_stream_->verify_item_and_move_to_next(\"{element.element_name}\");\n" \
                f"{indent_str}#ifndef NDEBUG\n" \
                f"{indent_str}\tstd::cout << \"setting value (\" + input_string_stream_->get_item() + \") for {element.element_type.type_name} '{element.element_name}' -> \" << std::endl;\n" \
                f"{indent_str}#endif\n" \
                f"{indent_str}{element.element_type.encode_function.call()};\n"
        return code

    def _encode_simple_element(self, element: Element, indent: int, from_optional=False) -> str:
        indent_str = "\t" * indent
        optional_str = "optional " if element.is_optional else ""
        if isinstance(element, Attribute):
            return f"{indent_str}{{  // encode {optional_str}simple Attribute type {element.element_type.type_name}\n" \
                   f"{self._encode_simple_type(element, indent + 1, from_optional)}" \
                   f"{indent_str}}}\n"
        return f"{indent_str}{{  // encode {optional_str}simple Element type {element.element_type.type_name}\n" \
               f"{indent_str}\tbase_types_->add_event_code(\"Start{element.element_name}\");\n" \
               f"{self._encode_simple_type(element, indent + 1, from_optional)}" \
               f"{indent_str}\tbase_types_->add_event_code(\"End{element.element_name}\");\n" \
               f"{indent_str}}}\n"

    def encode_list(self, element: Element, indent: int):
        def _encode_list_item():
            if element.element_type.is_simple_not_complex:
                return f"{indent_str}\tbase_types_->add_event_code(\"StartSimpleElement{element.element_type.type_name}\");\n" \
                       f"{indent_str}\t#ifndef NDEBUG\n" \
                       f"{indent_str}\t\tstd::cout << \"setting value (\" + input_string_stream_->get_item() + \") for {element.__class__.__name__} '{element.element_name}' -> \" << std::endl;\n" \
                       f"{indent_str}\t#endif\n" \
                       f"{indent_str}\t{element.element_type.encode_function.call()};\n" \
                       f"{indent_str}\tbase_types_->add_event_code(\"EndSimpleElement{element.element_type.type_name}\");\n"
            else:
                return f"{indent_str}\t\t{element.element_type.encode_function.call()};\n"

        if isinstance(element, Attribute):
            raise RuntimeError(f"List can not be of type Attribute! {element}")

        indent_str = "\t" * indent
        code = f"{indent_str}{{  // encode list Element type {element.element_type.type_name}\n" \
               f"{indent_str}\tinput_string_stream_->verify_item_and_move_to_next(\"{element.element_name}\");\n" \
               f"{indent_str}\tbase_types_->add_event_code(\"Start{element.element_name}List\");\n" \
               f"{indent_str}\tbool is_in_list = input_string_stream_->is_list_start_move_to_next();\n" \
               f"{indent_str}\tif (!is_in_list) {{\n" \
               f"{indent_str}\t\tthrow std::runtime_error(\"Expecting at least one item in list {element.element_type.type_name}\");\n" \
               f"{indent_str}\t}}\n" \
               f"{indent_str}\tfor(int i=0; i<{element.max_items}; i++) {{\n" \
               f"{indent_str}\t\tif (i!=0) {{\n" \
               f"{indent_str}\t\t\tis_in_list = !input_string_stream_->is_list_end_move_to_next();\n" \
               f"{indent_str}\t\t\tif (is_in_list) {{\n" \
               f"{indent_str}\t\t\t\tbase_types_->add_event_code_with_n_bits(0, 2, \"NextListItem_{element.element_name}\");\n" \
               f"{indent_str}\t\t\t}} else {{\n" \
               f"{indent_str}\t\t\t\tbase_types_->add_event_code_with_n_bits(1, 2, \"NoFurtherListItem_{element.element_name}\");\n" \
               f"{indent_str}\t\t\t\tbreak;\n" \
               f"{indent_str}\t\t\t}}\n" \
               f"{indent_str}\t\t}}\n" \
               f"{_encode_list_item()}" \
               f"{indent_str}\t}}\n" \
               f"{indent_str}}}\n"
        return code

    def _encode_complex_element(self, element: Element, indent: int, from_optional=False) -> str:
        indent_str = "\t" * indent
        optional_str = "optional " if element.is_optional else ""

        return_str = f"{indent_str}// encode {optional_str}complex element {element.element_name} with type {element.element_type.type_name}\n"
        if from_optional:
            return_str += f"{indent_str}//   without add_event_code(...) because previous element was optional;\n"
        else:
            return_str += f"{indent_str}base_types_->add_event_code(\"Start{element.element_name}\");\n"
        return_str += f"{indent_str}input_string_stream_->verify_item_and_move_to_next(\"{element.element_name}\");\n" \
                      f"{indent_str}{element.element_type.encode_function.call()};\n"
        return return_str

    def encode_element(self, element: Element, indent=0, from_optional=False) -> str:
        if element.element_type.is_simple_not_complex:
            return self._encode_simple_element(element, indent, from_optional)
        return self._encode_complex_element(element, indent, from_optional)

    def get_encode_code_for_optional_and_abstract_blob(self, elements: List[Element], is_last: bool) -> str:
        def get_num_elements():
            num_elements = len(all_elements_flat)
            if is_last:
                num_elements += 1
            return num_elements

        def get_lookup_table_range_str(while_loop_offset) -> str:
            return ",".join([str(i) for i in range(ComplexTypes.get_num_eventcode_bits(elements, is_last, 0),
                                                   ComplexTypes.get_num_eventcode_bits(elements, is_last, while_loop_offset)-1, -1)])

        all_elements_flat = ComplexTypes.get_all_elements_flat(elements)
        names = "_".join([e.element_name for e in all_elements_flat])
        end_str = " + END_ELEMENT" if is_last else ""
        code = f"{{  // Encoding optional elements: {[en.element_name for en in all_elements_flat]}{end_str}\n" \
               f"\t#ifndef NDEBUG\n" \
               f"\t\tstd::cout << \"optional value blob starting [{[en.element_name for en in all_elements_flat]}{end_str}] \" << std::endl;\n" \
               f"\t#endif\n" \
               f"\tstatic std::unordered_map<std::string, uint8_t> const table = {{\n"

        while_loop_offset = 0
        for i, element in enumerate(all_elements_flat):
            code += f"\t\t{{\"{element.element_name}\", {i}}},\n"

        code += f"\t}};\n" \
                f"\n" \
                f"\tuint8_t offset = 0, event_code, num_bits_eventcode;\n" \
                f"\tstd::string element_name;\n" \
                f"\tfor (int8_t iteration=0; iteration < {len(all_elements_flat)}; iteration++) {{\n" \
                f"\t\telement_name = input_string_stream_->get_item();\n" \
                f"\t\tauto it = table.find(element_name);\n" \
                f"\t\tif (it != table.end()) {{\n" \
                f"\t\t\tuint8_t index = it->second;\n" \
                f"\t\t\tswitch(index) {{\n"
        for i, element in enumerate(all_elements_flat):
            code += f"\t\t\t\tcase {i}:\n"
            while_loop_offset += ComplexTypes.get_num_abstract_elements_per_base(element)
            code += f"\t\t\t\t\tnum_bits_eventcode = log_lookup[std::max(static_cast<int8_t>(2), static_cast<int8_t>({get_num_elements()} - iteration))];\n" \
                    f"\t\t\t\t\tevent_code = index - offset;\n" \
                    f"\t\t\t\t\toffset = index + 1;\n" \
                    f"\t\t\t\t\tbase_types_->add_event_code_with_n_bits(event_code, num_bits_eventcode, \"OptionalElement_{element.element_name}\");\n"

            if element.is_list:
                code += self.encode_list(element, 5)
            else:
                code += f"{self.encode_element(element, 5, from_optional=True)}\n"

            if element.is_optional and ((i != (len(all_elements_flat)-1)) or is_last):
                code += f"\t\t\t\t\tcontinue;  // next optional value\n"
            else:
                code += f"\t\t\t\t\tbreak;  // exit loop\n"
        code += f"\t\t\t\tdefault:\n" \
                f"\t\t\t\t\tthrow std::runtime_error(\"The item \" +  std::to_string(index) + \" does not exist!\");\n" \
                f"\t\t\t}}\n" \
                f"\t\t\tbreak;  // exit loop\n" \
                f"\t\t}} else {{\n"

        code += f"\t\t\t// last element of type --> can not catch unexisting elements\n" \
                f"\t\t\tnum_bits_eventcode = {max(2, ComplexTypes.get_num_eventcode_bits(elements, is_last))};\n" \
                f"\t\t\tevent_code = {len(all_elements_flat)} - offset;\n" \
                f"\t\t\tbase_types_->add_event_code_with_n_bits(event_code, num_bits_eventcode, \"OptionalElement_\" + element_name);\n" \
                f"\t\t\tbreak;\n"
        code += f"\t\t}}\n" \
                f"\t}}\n" \
                f"}}\n"
        return code

    def getEncodeFunction(self, ct: ComplexType):
        code = ""
        optional_blob = []
        was_normal_complex = False

        for element in ct.child_elements:
            was_normal_complex = False
            if element.is_list:
                if element.is_optional:
                    optional_blob.append(element)
                else:
                    code += self.encode_list(element, indent=0)
                continue
            elif element.element_type.is_abstract and not element.is_optional:
                self.do_abstract_element_checks(element)
                optional_blob.append(element)
                # finish optional list with abstract types
                sorted_blob = ComplexTypes.sort_optional_and_abstract_elements(optional_blob)
                code += self.get_encode_code_for_optional_and_abstract_blob(sorted_blob, is_last=False)
                code += "\n"
                optional_blob = []
                continue
            elif element.is_optional:
                optional_blob.append(element)
                continue

            if len(optional_blob) != 0:
                sorted_blob = ComplexTypes.sort_optional_and_abstract_elements(optional_blob)
                code += self.get_encode_code_for_optional_and_abstract_blob(sorted_blob, is_last=False)
                code += "\n"
                optional_blob = []
            code += self.encode_element(element) + "\n"
            was_normal_complex = True

        if len(optional_blob) != 0:
            sorted_blob = ComplexTypes.sort_optional_and_abstract_elements(optional_blob)
            code += self.get_encode_code_for_optional_and_abstract_blob(sorted_blob, is_last=True)

        if was_normal_complex:
            code += f"base_types_->add_event_code(\"Finish{ct.type_name}\");\n"

        return CppFunction(function_name=ct.encode_function.function_name,
                           return_type="void",
                           arguments=None,
                           code=code,
                           comment=None)

    def write_complex_type_conversion_header(self, directory: str):
        self.cpp_class.write_h(directory)

    def write_complex_type_conversion_source(self, directory: str):
        self.cpp_class.write_cpp(directory)
