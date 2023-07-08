from utility import blender
from bmodule.material import helper
from bmodule.material import node_base
from bmodule.material import (
        output_nodes,
        input_nodes,
        surface_nodes,
        math_nodes)
from psdl import sdl
from bmodule import naming

import bpy
import nodeitems_utils

from collections import defaultdict


def to_sdl_recursive(b_material, current_node, processed_nodes, sdlconsole):
    for socket in current_node.inputs:
        for link in socket.links:
            from_node = link.from_node
            if from_node not in processed_nodes:
                to_sdl_recursive(b_material, from_node, processed_nodes, sdlconsole)
                processed_nodes.add(from_node)

    current_node.to_sdl(b_material, sdlconsole)


def to_sdl(b_material, sdlconsole):
    node_tree = helper.find_node_tree_from_material(b_material)
    output_node = helper.find_output_node_from_node_tree(node_tree)
    if output_node is None:
        print("material <%s> has no output node, generating fallback material for it" % b_material.name)
        fallback_creator = sdl.MatteOpaqueMaterialCreator()
        fallback_creator.set_data_name(naming.get_mangled_material_name(b_material))
        sdlconsole.queue_command(fallback_creator)
        return

    processed_nodes = set()
    to_sdl_recursive(b_material, output_node, processed_nodes, sdlconsole)


PH_MATERIAL_NODES = [
        output_nodes.PhOutputNode,
        input_nodes.PhFloatValueInputNode,
        input_nodes.PhConstantColorInputNode,
        input_nodes.PhPictureNode,
        surface_nodes.PhDiffuseSurfaceNode,
        surface_nodes.PhBinaryMixedSurfaceNode,
        surface_nodes.PhAbradedOpaqueNode,
        surface_nodes.PhAbradedTranslucentNode,
        surface_nodes.PhLayeredSurfaceNode,
        surface_nodes.PhSurfaceLayerNode,
        surface_nodes.PhIdealSubstanceNode,
        surface_nodes.PhPureAbsorberNode,
        math_nodes.PhMultiplyNode,
        math_nodes.PhAddNode]


@blender.register_module
class MaterialNodes(blender.BlenderModule):
    node_category_idname = 'PH_MATERIAL_NODE_CATEGORIES'

    def __init__(self):
        super().__init__()

        self.b_node_categories = []

        node_category_to_items = defaultdict(list)
        for node_class in PH_MATERIAL_NODES:
            node_category = node_class.node_category
            if node_category is not None:
                node_category_to_items[node_category].append(nodeitems_utils.NodeItem(node_class.bl_idname))
            else:
                print("error: node class <%s> has no \"node_category\" class attribute which is required" % (
                        node_class.__name__))

        for node_category, items in node_category_to_items.items():
            b_node_category = node_base.PhMaterialNodeCategory(
                    node_category.id_name,
                    node_category.label,
                    items=items)
            self.b_node_categories.append(b_node_category)

    def register(self):
        for node_type in PH_MATERIAL_NODES:
            bpy.utils.register_class(node_type)

        nodeitems_utils.register_node_categories(self.node_category_idname, self.b_node_categories)

    def unregister(self):
        for node_type in PH_MATERIAL_NODES:
            bpy.utils.unregister_class(node_type)

        nodeitems_utils.unregister_node_categories(self.node_category_idname)
