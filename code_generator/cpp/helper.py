import re


def CamelCase2snake_case(camel):
    return re.sub(r'(?<!^)(?=[A-Z])', '_', camel).lower()
