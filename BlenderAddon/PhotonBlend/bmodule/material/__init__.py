from bmodule.material import properties
from bmodule.material import nodes
from bmodule.material import node_base
from bmodule.material import helper


def include_module(module_manager):
    properties.include_module(module_manager)
    node_base.include_module(module_manager)
    nodes.include_module(module_manager)
