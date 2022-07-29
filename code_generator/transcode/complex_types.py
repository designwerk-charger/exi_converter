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
                                           "#include <map>\n"
                                           "#include \"type_conversion/base_types.h\"\n#include \"enum_types.h\"\n"
                                           "#include \"base/output_string_stream.h\"\n"
                                           "#include \"base/json_parser.h\"\n",
                                  namespace=namespace)
        self.cpp_class.add_member("BaseTypes * base_types_;\n\tEnumTypes * enum_types_;\n"
                                  "\tOutputStringStream * output_string_stream_;\n"
                                  "\tenum { arrsize = 16 };\n"
                                  "\tstatic constexpr uint8_t log_lookup[arrsize] = {1, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4};\n")
        self.cpp_class.add_constructor("BaseTypes * base_types, EnumTypes * enum_types, OutputStringStream * output_string_stream",
                                       "base_types_ = base_types;\nenum_types_ = enum_types;\n"
                                       "output_string_stream_ = output_string_stream;\n\n")
        self.cpp_class.add_constructor("BaseTypes * base_types, EnumTypes * enum_types",
                                       "base_types_ = base_types;\nenum_types_ = enum_types;\n"
                                       "output_string_stream_ = nullptr;\n")

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
                    all_elements_flat.append(Element(element_name=element_name, element_type=element_type,
                                                     is_optional=e.is_optional, max_items=1, substitutes={},
                                                     created_from_abstract=True))
            elif e.is_optional:
                all_elements_flat.append(e)
        return all_elements_flat

    @staticmethod
    def get_all_elements_flat2(elements: List[Element]):
        cnt = -1
        all_elements_flat = []
        counter_value = []
        for e in elements:
            if e.element_type.is_abstract:
                cnt += len(e.substitutes.items())
                for (element_name, element_type) in e.substitutes.items():
                    all_elements_flat.append(Element(element_name=element_name, element_type=element_type,
                                                     is_optional=e.is_optional, max_items=1, substitutes={},
                                                     created_from_abstract=True))
                    counter_value.append(cnt)
            else:
                cnt += 1
                counter_value.append(cnt)
                all_elements_flat.append(e)
        return all_elements_flat, counter_value

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

        code += f"{indent_str}const std::string & value = (*json)[\"{element.element_name}\"]->get_value();\n" \
                f"{indent_str}#ifndef NDEBUG\n" \
                f"{indent_str}\tstd::cout << \"setting value (\" + value + \") for {element.element_type.type_name} '{element.element_name}' -> \" << std::endl;\n" \
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
                return f"{indent_str}\t\tstd::string value = array[i]->get_value();\n" \
                       f"{indent_str}\t\tbase_types_->add_event_code(\"StartSimpleElement{element.element_type.type_name}\");\n" \
                       f"{indent_str}\t\t#ifndef NDEBUG\n" \
                       f"{indent_str}\t\t\tstd::cout << \"setting value (\" + value + \") for {element.__class__.__name__} '{element.element_name}' -> \" << std::endl;\n" \
                       f"{indent_str}\t\t#endif\n" \
                       f"{indent_str}\t\t{element.element_type.encode_function.call()};\n" \
                       f"{indent_str}\t\tbase_types_->add_event_code(\"EndSimpleElement{element.element_type.type_name}\");\n"
            else:
                return f"{indent_str}\t\tstd::shared_ptr<JObject> obj = std::dynamic_pointer_cast<JObject>(array[i]);\n" \
                       f"{indent_str}\t\t{element.element_type.encode_function.call('obj')};\n"

        if isinstance(element, Attribute):
            raise RuntimeError(f"List can not be of type Attribute! {element}")

        indent_str = "\t" * indent
        code = f"{indent_str}{{  // encode list Element type {element.element_type.type_name}\n" \
               f"{indent_str}\tconst auto & array = (*json)[\"{element.element_name}\"]->get_array();\n" \
               f"{indent_str}\tbase_types_->add_event_code(\"Start{element.element_name}List\");\n" \
               f"{indent_str}\tif (array.size() > {element.max_items}) [[unlikely]] {{\n" \
               f"{indent_str}\t\tthrow std::runtime_error(\"There are more items in the list than specified for {element.element_type.type_name}\");\n" \
               f"{indent_str}\t}}\n" \
               f"{indent_str}\tif (array.size() < 1) [[unlikely]] {{\n" \
               f"{indent_str}\t\tthrow std::runtime_error(\"The list is empty which is not implemented for {element.element_type.type_name}\");\n" \
               f"{indent_str}\t}}\n" \
               f"{indent_str}\tfor (int i=0; i < array.size(); i++) {{\n" \
               f"{_encode_list_item()}" \
               f"{indent_str}\t\tif (array.size() - 1 > i) {{\n" \
               f"{indent_str}\t\t\tbase_types_->add_event_code_with_n_bits(0, 2, \"NextListItem_{element.element_name}\");\n" \
               f"{indent_str}\t\t}} else if ({element.max_items} - 1 == i) {{\n" \
               f"{indent_str}\t\t\tbreak;  // no further items in list\n" \
               f"{indent_str}\t\t}} else {{\n" \
               f"{indent_str}\t\t\tbase_types_->add_event_code_with_n_bits(1, 2, \"NoFurtherListItem_{element.element_name}\");\n" \
               f"{indent_str}\t\t\tbreak;\n" \
               f"{indent_str}\t\t}}\n" \
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
        return_str += f"{indent_str}{{\n" \
                      f"{indent_str}\tstd::shared_ptr<JObject> jobject = json->get_object(\"{element.element_name}\");\n" \
                      f"{indent_str}\t{element.element_type.encode_function.call()};\n" \
                      f"{indent_str}}}"
        return return_str

    def encode_element(self, element: Element, indent=0, from_optional=False) -> str:
        if element.element_type.is_simple_not_complex:
            return self._encode_simple_element(element, indent, from_optional)
        return self._encode_complex_element(element, indent, from_optional)

    def get_encode_code_for_optional_and_abstract_blob(self, elements: List[Element], is_last: bool) -> str:
        def get_num_elements():
            num_elements = len(all_elements_flat) + ComplexTypes.get_num_AnyElements(elements) + 1
            if is_last:
                num_elements += 1
            return max(2, num_elements)

        def get_element_description_line(element: Element):
            abstract_str = f"derived from abstract base {element.element_type.base_type_name}" if element.created_from_abstract else ""
            optional_str = f"optional " if element.is_optional else ""
            list_str = f"list " if element.is_list else ""
            element_type = element.__class__.__name__
            return f"// {optional_str} {list_str} {element_type} '{element.element_name}:{element.element_type.type_name}' {abstract_str}"

        def get_num_bits_eventcode_regarding_abstract(i: int) -> int:
            num_elements = get_num_elements()
            num_elements -= counter_values[i]
            if not elements[-1].is_optional or is_last:
                num_elements -= 1
            return ceil(log2(max(1, num_elements)))

        all_elements_flat, counter_values = ComplexTypes.get_all_elements_flat2(elements)
        end_str = " + END_ELEMENT" if is_last else ""
        code = f"{{  // Encoding optional elements: {[en.element_name for en in all_elements_flat]}{end_str}\n" \
               f"\t#ifndef NDEBUG\n" \
               f"\t\tstd::cout << \"optional value blob starting [{[en.element_name for en in all_elements_flat]}{end_str}] \" << std::endl;\n" \
               f"\t#endif\n" \
               f"\tstatic std::unordered_map<std::string, uint8_t> const table = {{\n"

        for i, element in reversed(list(enumerate(all_elements_flat))):
            code += f"\t\t{{\"{element.element_name}\", {i}}},\n"

        code += f"\t}};\n" \
                f"\n" \
                f"\tuint8_t offset = 0, iteration = 0, event_code, num_bits_eventcode;\n" \
                f"\tstd::string element_name;\n" \
                f"\tbool last_option_taken = false; bool no_element_taken = true;\n" \
                f"\tnum_bits_eventcode = {max(2, ceil(log2(get_num_elements())))};\n" \
                f"\tif (json.get() != nullptr) [[likely]] {{\n" \
                f"\tfor (const auto & it: table) {{\n" \
                f"\t\titeration++;\n" \
                f"\t\tconst auto & current_option = it.first;\n" \
                f"\t\tif (json->exists(current_option)) {{\n" \
                f"\t\t\tuint8_t index = it.second;\n" \
                f"\t\t\tswitch(index) {{\n"
        for i, element in enumerate(all_elements_flat):
            code += f"\t\t\t\tcase {i}:\n" \
                    f"\t\t\t\t\t{get_element_description_line(element)}\n"
            if element.is_optional or element.created_from_abstract:
                code += f"\t\t\t\t\tevent_code = index - offset;\n" \
                        f"\t\t\t\t\toffset = index + {self.get_num_abstract_elements_per_base(element)};\n" \
                        f"\t\t\t\t\tbase_types_->add_event_code_with_n_bits(event_code, num_bits_eventcode, \"OptionalElement_{element.element_name}\");\n" \
                        f"\t\t\t\t\tnum_bits_eventcode = {get_num_bits_eventcode_regarding_abstract(i)};\n"
            else:
                code += f"\t\t\t\t\tevent_code = index - offset;\n" \
                        f"\t\t\t\t\tbase_types_->add_event_code_with_n_bits(event_code, num_bits_eventcode, \"NonOptionalElement_{element.element_name}\");\n"

            if element.is_list:
                code += self.encode_list(element, 5)
            else:
                code += f"{self.encode_element(element, 5, from_optional=True)}\n"

            if element.created_from_abstract:
                 code += f"\t\t\t\t\t// this element is abstract. The optional blob needs to end\n" \
                         f"\t\t\t\t\tlast_option_taken = true;\n"
            elif i == (len(all_elements_flat)-1):
                code += f"\t\t\t\t\tlast_option_taken = true;\n"
            code += f"\t\t\t\t\tbreak;\n"
        code += f"\t\t\t\tdefault:\n" \
                f"\t\t\t\t\tthrow std::runtime_error(\"The item \" +  std::to_string(index) + \" does not exist!\");\n" \
                f"\t\t\t}}\n" \
                f"\t\t\tno_element_taken = false;\n" \
                f"\t\t}}}}\n" \
                f"\t}}\n"

        code += f"\tif (!last_option_taken) {{\n" \
                f"\t\tevent_code = {len(all_elements_flat)} - offset;\n" \
                f"\t\tbase_types_->add_event_code_with_n_bits(event_code, num_bits_eventcode, \"EndingOptionalElement_{element.element_name}\");\n" \
                f"\t}}"
        if is_last:
            code += f" else if (last_option_taken) {{\n" \
                    f"\t\tbase_types_->add_event_code_with_n_bits(0, 1, \"EndingElement_{element.element_name}\");\n" \
                    f"\t}}"
        code += f"\n}}\n"

        return code

    def getEncodeFunction(self, ct: ComplexType):
        code = ""
        optional_blob = []
        was_normal_complex = False

        is_last = False
        for element in ct.child_elements:
            is_last = ct.child_elements[-1] == element
            was_normal_complex = False
            if element.is_list:
                if element.is_optional:
                    optional_blob.append(element)
                elif len(optional_blob) != 0:
                    # finish optional list and decode list
                    sorted_blob = ComplexTypes.sort_optional_and_abstract_elements(optional_blob)
                    code += self.get_encode_code_for_optional_and_abstract_blob(sorted_blob, is_last=False)
                    code += "\n"
                    code += self.encode_list(element, 0)
                    optional_blob = []
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
                optional_blob.append(element)
                sorted_blob = ComplexTypes.sort_optional_and_abstract_elements(optional_blob)
                code += self.get_encode_code_for_optional_and_abstract_blob(sorted_blob, is_last=False)
                code += "\n"
                # was_optional = optional_blob[-1].is_optional and not optional_blob[-1].is_list
                optional_blob = []
                was_normal_complex = True # isinstance(element, Attribute)
            else:
                code += self.encode_element(element, from_optional=False) + "\n"
                was_normal_complex = True

        if len(optional_blob) != 0:
            sorted_blob = ComplexTypes.sort_optional_and_abstract_elements(optional_blob)
            code += self.get_encode_code_for_optional_and_abstract_blob(sorted_blob, is_last=is_last)

        if was_normal_complex:
            code += f"base_types_->add_event_code(\"Finish{ct.type_name}\");\n"

        return CppFunction(function_name=ct.encode_function.function_name,
                           return_type="void",
                           arguments="const std::shared_ptr<JObject> & json",
                           code=code,
                           comment=None)

    def write_complex_type_conversion_header(self, directory: str):
        self.cpp_class.write_h(directory)

    def write_complex_type_conversion_source(self, directory: str):
        self.cpp_class.write_cpp(directory)
