import abc


class BaseType(metaclass=abc.ABCMeta):

    def __init__(self, type_name: str, type_namespace: str):
        self.is_simple_not_complex = None
        self.is_abstract = False
        self.base_class_name = ""
        self.type_name = type_name
        self.type_namespace = type_namespace

    @property
    def base_type_name(self):
        return self.__class__.__name__

    def __eq__(self, other):
        if isinstance(other, str):
            return self.type_namespace == other
        elif isinstance(other, BaseType):
            return self.type_namespace == other.type_namespace
        raise NotImplementedError(f"Cannot compare {other.__class__.__name__} with {self.__class__.__name__}!")

    @property
    @abc.abstractmethod
    def encode_function(self):
        raise NotImplementedError

    @property
    @abc.abstractmethod
    def decode_function(self):
        raise NotImplementedError

    @abc.abstractmethod
    def get_simple_types(self):
        raise NotImplementedError

    @abc.abstractmethod
    def get_child_types(self):
        raise NotImplementedError
