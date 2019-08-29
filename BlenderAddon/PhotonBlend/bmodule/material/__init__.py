from . import properties
from . import node


def include_module(module_manager):
    properties.include_module(module_manager)
    node.include_module(module_manager)
