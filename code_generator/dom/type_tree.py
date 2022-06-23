import collections
from typing import List

import xmlschema
from xmlschema import XsdElement, XsdAttribute, XsdType, XsdComponent
from xmlschema.validators import XsdAnyElement, XsdGroup

from datatypes.base_type import BaseType
from datatypes.complex_type import ComplexType
from datatypes.element import Element, Attribute, AnyElement
from datatypes.simple_type import SimpleType, EnumType, StringType, DecimalType, BoolType, HexBinType, Base64Type, \
    IgnoredType, UriType, NBitDecimalType

from dom.ccs_messages import MESSAGE_TYPES, ADDITIONAL_TYPES


class TypeTree:
    IGNORED_TYPES = ["xs:fullDerivationSet", "xs:simpleDerivationSet", 'xs:derivationSet', 'xs:allNNI', 'xs:blockSet',
                     'xs:namespaceList', 'xs:float', 'xs:double', 'xs:duration', 'xs:time', 'xs:date', 'xs:dateTime',
                     'xs:gYearMonth', 'xs:gYear', 'xs:gMonth', 'xs:gDay', 'xs:gMonthDay', 'xs:QName', 'xs:NOTATION',
                     'xs:string+', 'xs:anySimpleType']

    def __init__(self, schema_file: str, namespace: str):
        self._schema = xmlschema.XMLSchema(source=schema_file)

        self._all_types = TypeTree.extract_all_types(self._schema.types.target_dict.values())
        TypeTree.update_derivations(self._all_types)

        self._type_tree = []
        top_level_msgs = self.get_full_list_of_message_types(MESSAGE_TYPES[namespace]) + ADDITIONAL_TYPES[namespace]
        top_level_msgs.append("V2G_Message")
        for name, types in self._schema.types.target_dict.items():
            if types.local_name in top_level_msgs:
                self._type_tree.append(self.diveIntoType(types, self._all_types))

        print(f"Type Tree created with {len(self._type_tree)} top level types")

    @staticmethod
    def extract_all_types(schema_types: List[XsdType]) -> List[BaseType]:
        all_types = []
        for t in schema_types:
            if t.is_simple():
                all_types.append(TypeTree.getSimpleTypeInfo(t))
            else:
                base_type = None if t.base_type == None else t.base_type.local_name
                all_types.append(ComplexType(t.local_name, t.qualified_name, base_type, t.abstract))
        return all_types

    @staticmethod
    def update_derivations(all_types: List[BaseType]):
        for type in all_types:
            if type.is_abstract and not isinstance(type, ComplexType):
                raise RuntimeError(f"The type ({type}) is abstract but not complex!")
            for type2 in all_types:
                if type.type_name == type2.base_class_name:
                    if not isinstance(type2, ComplexType):
                        raise RuntimeError(f"The type ({type2}) is not complex!")
                    if not type.is_simple_not_complex:
                        type2.add_base_class(type)
                        type.derived_classes.append(type2)
                    else:
                        print(f"INFO: Found simple BaseClass {type.type_namespace} for complex type {type2.type_name} --> adding as baseclass")
                        type2.add_base_class(type)

    @property
    def schema(self):
        return self._schema

    @property
    def type_tree(self) -> List[BaseType]:
        return self._type_tree

    @staticmethod
    def get_full_list_of_message_types(msgs: List):
        full_list = []
        for msg in msgs:
            full_list.append(f"{msg}ResType")
            full_list.append(f"{msg}ReqType")
        return full_list

    @staticmethod
    def extractComponentType(c: XsdComponent):
        if c.type.qualified_name in c.schema.types.target_dict.keys():
            return c.schema.types.target_dict[c.type.qualified_name]
        if c.type.base_type and c.type.base_type.qualified_name in c.schema.types.target_dict.keys():
            return c.schema.types.target_dict[c.type.base_type.qualified_name]
        if c.type.primitive_type.qualified_name in c.schema.types.target_dict.keys():
            return c.schema.types.target_dict[c.type.primitive_type.qualified_name]
        else:
            raise KeyError(f"The type ({c.type.qualified_name}) for {c.name} does not exist.")

    @staticmethod
    def getSimpleTypeInfo(t: XsdType) -> SimpleType:
        if not t.is_simple():
            raise TypeError(f"The type {t.qualified_name} is not simple as expected!")

        if t.sequence_type == "xs:string":
            if t.enumeration:
                return EnumType(t.prefixed_name, type_namespace=t.qualified_name, enumerations=t.enumeration)
            else:
                return StringType(t.local_name, type_namespace=t.qualified_name)
        elif t.sequence_type == "xs:decimal":
            try:
               return DecimalType(t.local_name, type_namespace=t.qualified_name, detail_type=t.local_name, min_val=t.min_value, max_val=t.max_value)
            except Exception:
                print(f"INFO: {t.local_name} could not be interpreted as pure decimal!")
                return NBitDecimalType(t.local_name, type_namespace=t.qualified_name, detail_type=t.local_name, min_val=t.min_value, max_val=t.max_value)
        elif t.sequence_type == "xs:boolean":
            return BoolType(t.local_name, type_namespace=t.qualified_name)
        elif t.sequence_type == "xs:hexBinary":
            return HexBinType(t.local_name, type_namespace=t.qualified_name, max_length=t.max_length)
        elif t.sequence_type == "xs:base64Binary":
            return Base64Type(t.local_name, type_namespace=t.qualified_name)
        elif t.sequence_type == "xs:anyURI":
            return UriType(t.local_name, type_namespace=t.qualified_name)
        elif t.sequence_type in TypeTree.IGNORED_TYPES:
            return IgnoredType(type_namespace=t.qualified_name)
        else:
            raise TypeError(f"The type {t.qualified_name} ist not supported! {t}")

    @staticmethod
    def get_substitutes(element: XsdElement, all_types: List[BaseType]):
        stt = {}
        for s in element.iter_substitutes():
            stt[s.local_name] = TypeTree.diveIntoType(s.type, all_types)
        if len(stt) > 0:
            # if substitutes available, the base class needs to be added to.
            stt[element.local_name] = TypeTree.diveIntoType(element.type, all_types)
        return collections.OrderedDict(sorted(stt.items()))

    @staticmethod
    def diveIntoType(t: XsdType, all_types: List[BaseType]) -> BaseType:
        if t.qualified_name not in all_types:
            raise RuntimeError(f"The type {t.qualified_name} is not available in available types")

        refered_item = all_types[all_types.index(t.qualified_name)]
        if t.is_simple():
            return refered_item
        elif isinstance(refered_item, ComplexType):
            if refered_item.base_class_name:
                # dive into base types to update their components
                TypeTree.diveIntoType(t.base_type, all_types)

            components = []
            optional_group = False
            for c in t.iter_components():
                if isinstance(c, XsdGroup):
                    if c.model == "choice":
                        optional_group = c.min_occurs == 0
                if isinstance(c, XsdElement):
                    is_optional = True if c.min_occurs == 0 else optional_group
                    xsd_type = TypeTree.extractComponentType(c)
                    substitutes = TypeTree.get_substitutes(c, all_types)
                    components.append(Element(c.local_name,
                                              TypeTree.diveIntoType(xsd_type, all_types),
                                              is_optional=is_optional, max_items=c.max_occurs, substitutes=substitutes))
                elif isinstance(c, XsdAnyElement):
                    is_optional = True if c.min_occurs == 0 else optional_group
                    if not is_optional:
                        print(f"WARNING: any element {c} in {t.local_name} is not optional!")
                    components.append(AnyElement("genericname",
                                                 StringType(type_name="anyname", type_namespace=c.namespace[0]),
                                                 is_optional=True))
                elif isinstance(c, XsdAttribute):
                    optional = True if c.use == "optional" else False
                    attr_type = TypeTree.diveIntoType(TypeTree.extractComponentType(c), all_types)
                    if not isinstance(attr_type, SimpleType):
                        raise TypeError(f"Attributes can only be simple types! got type '{attr_type.type_name}' for "
                                        f"component '{c.local_name}' in '{t.local_name}'")
                    if len(components) > 0 and not isinstance(components[-1], Attribute):
                        raise RuntimeError(f"Attributes are not allowed after Elements! "
                                           f"component '{c.local_name}' in '{t.local_name}'")
                    if len(components) > 0 and components[-1].element_name > c.local_name:
                        raise RuntimeError(f"Attributes are not lexicographical ordered! "
                                           f"new component '{c.local_name}' vs. "
                                           f"existing component '{components[-1].element_name}' in '{t.local_name}'")
                    components.append(Attribute(c.local_name, attr_type, is_optional=optional))
            if refered_item.base_class and isinstance(refered_item.base_class, SimpleType):
                components.append(Attribute(element_name="value", element_type=refered_item.base_class,
                                          is_optional=False))
            if len(components) == 0:
                print(f"WARNING: Complex Element without components {refered_item.type_name}")
            elif isinstance(components[-1], Attribute):
                print(f"Attribute only component in Type {refered_item.type_name}")
            refered_item.add_child_elements(components)
            return refered_item
        raise RuntimeError(f"the Type {t.local_name} is neither simple nor complex!")
