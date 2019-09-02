from . import properties
from . import nodes
from . import node_base


def include_module(module_manager):
    properties.include_module(module_manager)
    node_base.include_module(module_manager)
    nodes.include_module(module_manager)
