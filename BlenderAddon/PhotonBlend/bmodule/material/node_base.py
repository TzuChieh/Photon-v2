"""
Basic definitions and data for node-based materials.
"""

from utility import settings, blender
from bmodule import naming
from bmodule.material import helper

import bpy
import nodeitems_utils

from abc import abstractmethod


@blender.register_class
class PhMaterialNodeTree(bpy.types.NodeTree):
    bl_idname = 'PH_MATERIAL_NODE_TREE'
    bl_label = "Photon Node Tree"
    bl_icon = 'MATERIAL'

    COMPATIBLE_ENGINES = {settings.renderer_id_name}

    @classmethod
    def poll(cls, b_context):
        render_settings = b_context.scene.render
        return render_settings.engine in cls.COMPATIBLE_ENGINES

    # Blender: set the current node tree to the one the active material owns (update editor views)
    @classmethod
    def get_from_context(cls, b_context):
        b_material = helper.find_active_material_from_context(b_context)
        b_node_tree = helper.find_node_tree_from_material(b_material)
        if b_material is not None and b_node_tree is not None:
            return b_node_tree, b_material, b_material

        return None, None, None


class PhMaterialNodeSocket(bpy.types.NodeSocket):
    bl_idname = 'PH_MATERIAL_NODE_SOCKET'
    bl_label = "Photon Socket"

    link_only: bpy.props.BoolProperty(
        name="Link Only",
        description="Makes this node for linking only, its contained value(s) is ignored.",
        default=False
    )

    # Blender: draw socket's color
    def draw_color(self, b_context, node):
        return [0.0, 0.0, 0.0, 1.0]

    # Blender: draw socket
    def draw(self, b_context, b_layout, node, text):
        if node.bl_idname != 'PH_OUTPUT':
            if self.is_linked or self.is_output:
                b_layout.label(text=text)
            else:
                if hasattr(self, 'default_value'):
                    b_layout.prop(self, 'default_value', text=text)
                else:
                    b_layout.label(text=text)
        else:
            b_layout.label(text=text)

    def get_from_res_name(self, b_material, link_index=0):
        if not self.links:
            return None

        from_socket = self.links[link_index].from_socket
        return naming.get_mangled_output_node_socket_name(from_socket, b_material)


class NodeCategory:
    def __init__(self, id_name, label):
        self.id_name = id_name
        self.label = label

    def __hash__(self):
        return hash((self.id_name, self.label))

    def __eq__(self, other):
        return (self.id_name, self.label) == (other.id_name, other.label)


class PhMaterialNode(bpy.types.Node):
    bl_idname = 'PH_MATERIAL_NODE'
    bl_label = "Photon Node"
    bl_icon = 'MATERIAL'
    node_category = None

    @abstractmethod
    def to_sdl(self, b_material, sdlconsole):
        pass

    @classmethod
    def poll(cls, b_node_tree):
        return b_node_tree.bl_idname == PhMaterialNodeTree.bl_idname

    # Blender: called when node created
    def init(self, b_context):
        pass

    # Blender: draw properties in node
    def draw_buttons(self, b_context, b_layout):
        pass


@blender.register_class
class PhSurfaceMaterialSocket(PhMaterialNodeSocket):
    bl_idname = 'PH_SURFACE_MATERIAL_SOCKET'
    bl_label = "Surface Material"

    default_value: bpy.props.FloatVectorProperty(
        name="Albedo",
        description="Default constant albedo",
        default=[0.0, 0.0, 0.0],
        min=0.0,
        max=1.0,
        subtype='COLOR',
        size=3
    )

    def draw_color(self, b_context, node):
        return [0.8, 0.1, 0.1, 1.0]  # red


@blender.register_class
class PhFloatValueSocket(PhMaterialNodeSocket):
    bl_idname = 'PH_FLOAT_VALUE_SOCKET'
    bl_label = "Value"

    default_value: bpy.props.FloatProperty(
        name="Float",
        default=0.5,
        min=-1e32,
        max=1e32,
        subtype='NONE'
    )

    def draw_color(self, b_context, node):
        return [0.5, 0.5, 0.5, 1.0]  # gray


@blender.register_class
class PhFloatFactorSocket(PhMaterialNodeSocket):
    bl_idname = 'PH_FLOAT_SOCKET'
    bl_label = "Factor"

    default_value: bpy.props.FloatProperty(
        name="Float",
        default=0.5,
        min=0.0,
        max=1.0,
        subtype='FACTOR'
    )

    def draw_color(self, b_context, node):
        return [0.5, 0.5, 0.5, 1.0]  # gray


@blender.register_class
class PhColorSocket(PhMaterialNodeSocket):
    bl_idname = 'PH_COLOR_SOCKET'
    bl_label = "Color"

    default_value: bpy.props.FloatVectorProperty(
        name="Color",
        description="color value",
        default=[0.5, 0.5, 0.5],
        min=0.0,
        max=1.0,
        subtype='COLOR',
        size=3
    )

    def draw_color(self, b_context, node):
        return [0.7, 0.7, 0.1, 1.0]  # yellow


@blender.register_class
class PhSurfaceLayerSocket(PhMaterialNodeSocket):
    bl_idname = 'PH_SURFACE_LAYER_SOCKET'
    bl_label = "Surface Layer"

    def draw_color(self, b_context, node):
        return [0.0, 0.0, 0.0, 1.0]  # black


class PhMaterialNodeCategory(nodeitems_utils.NodeCategory):
    @classmethod
    def poll(cls, b_context):
        return b_context.space_data.tree_type == PhMaterialNodeTree.bl_idname


@blender.register_class
class PH_MATERIAL_HT_header(bpy.types.Header):
    bl_space_type = "NODE_EDITOR"

    def draw(self, b_context):
        b_layout = self.layout
        obj = b_context.object

        # TODO: remove node tree selection menu and prepend material.new like cycles

        if obj and obj.type not in {"LIGHT", "CAMERA"}:
            row = b_layout.row()

            # Show material.new when no active material exists
            row.template_ID(obj, "active_material", new="material.new")


OUTPUT_CATEGORY = NodeCategory("OUTPUT", "Output")
INPUT_CATEGORY = NodeCategory("INPUT", "Input")
SURFACE_MATERIAL_CATEGORY = NodeCategory("SURFACE", "Surface Material")
MATH_CATEGORY = NodeCategory("MATH", "Math")


def include_module(module_manager):
    pass
