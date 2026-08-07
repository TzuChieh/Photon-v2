from utility import blender, material
from bmodule.material import node_base
from bmodule.material import (
    output_nodes,
    input_nodes,
    surface_nodes,
    volume_nodes,
    math_nodes,
    conversion_nodes,
    group_nodes,
    )

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
    node_tree = material.find_node_tree_from_material(b_material)
    output_node = material.find_output_node_from_node_tree(node_tree)
    if output_node is None:
        print(f"material {b_material.name} has no output node, generating fallback material for it")
        node_base.queue_fallback_material(b_material, sdlconsole)
        return

    processed_nodes = set()
    to_sdl_recursive(b_material, output_node, processed_nodes, sdlconsole)


class PhMaterialNodeCategory(nodeitems_utils.NodeCategory):
    @classmethod
    def poll(cls, b_context):
        return b_context.space_data.tree_type == node_base.PhMaterialNodeTree.bl_idname


@blender.register_module
class MaterialNodes(blender.BlenderModule):
    node_category_idname = 'PH_MATERIAL_NODE_CATEGORIES'

    node_types = [
        output_nodes.PhOutputNode,
        input_nodes.PhFloatValueInputNode,
        input_nodes.PhConstantVectorInputNode,
        input_nodes.PhConstantColorInputNode,
        input_nodes.PhPictureNode,
        input_nodes.PhBlackBodyInputNode,
        input_nodes.PhAttributeInputNode,
        input_nodes.PhNoiseInputNode,
        surface_nodes.PhDiffuseSurfaceNode,
        surface_nodes.PhBinaryMixedSurfaceNode,
        surface_nodes.PhAbradedOpaqueNode,
        surface_nodes.PhAbradedTranslucentNode,
        surface_nodes.PhLayeredSurfaceNode,
        surface_nodes.PhSurfaceLayerNode,
        surface_nodes.PhNormalMappedSurfaceNode,
        surface_nodes.PhIdealSubstanceNode,
        surface_nodes.PhPureAbsorberNode,
        surface_nodes.PhThinDielectricSurfaceNode,
        volume_nodes.PhIdealMediumNode,
        math_nodes.PhArithmeticNode,
        math_nodes.PhClampNode,
        conversion_nodes.PhColorRemapNode,
        conversion_nodes.PhHsvNode,
        conversion_nodes.PhSplitImageNode,
        conversion_nodes.PhLuminanceNode,
        conversion_nodes.PhTransformedImageNode,
        group_nodes.PhGroupNode,
        ]
        
    operator_types = [
        node_base.PH_MATERIAL_OT_make_node_group
        ]

    def __init__(self):
        super().__init__()
        self.node_categories = []

        node_category_to_items = defaultdict(list)

        # Categorize node types
        for node_class in self.node_types:
            node_category = node_class.node_category
            if node_category is not None:
                node_category_to_items[node_category].append(nodeitems_utils.NodeItem(node_class.bl_idname))
            else:
                print(f"error: node class {node_class.__name__} has no \"node_category\" class attribute which is required")
                
        # Node group category
        node_group_category = node_base.PhMaterialGroupNode.node_category
        node_category_to_items[node_group_category].append(nodeitems_utils.NodeItemCustom(draw=self.node_category_draw_groups))

        for node_category, items in node_category_to_items.items():
            b_node_category = PhMaterialNodeCategory(
                node_category.id_name,
                node_category.label,
                items=items)
            self.node_categories.append(b_node_category)

    @staticmethod
    def context_menu_draw_operators(self, b_context):
        """
        NOTE: This is a static method, and will be called with an explicit `self`.
        """
        b_layout = self.layout
        b_layout.separator()
        for operator_type in MaterialNodes.operator_types:
            b_layout.operator(operator_type.bl_idname, text=operator_type.bl_label)

    @staticmethod
    def node_category_draw_groups(self, b_layout, b_context):
        """
        NOTE: This is a static method, and will be called with an explicit `self`.
        """
        # `node_groups` are actually node trees. A node tree can be wrapped by a "group" node when added to another node tree.
        for b_node_tree in bpy.data.node_groups:
            if b_node_tree.bl_idname == node_base.PhMaterialNodeTree.bl_idname:
                b_layout.operator("bpy.ops.node.add_node", text=b_node_tree.name)

    def register(self):
        for node_class in self.node_types:
            bpy.utils.register_class(node_class)

        nodeitems_utils.register_node_categories(self.node_category_idname, self.node_categories)

        # Show operators in the right-click context menu
        bpy.types.NODE_MT_context_menu.append(self.context_menu_draw_operators)

    def unregister(self):
        nodeitems_utils.unregister_node_categories(self.node_category_idname)

        for node_class in self.node_types:
            bpy.utils.unregister_class(node_class)

        bpy.types.NODE_MT_context_menu.remove(self.context_menu_draw_operators)
