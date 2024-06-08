"""
Basic definitions and data for node-based materials.
"""

from utility import settings, blender, material
from bmodule import naming

import bpy
import nodeitems_utils


class PhMaterialNodeSocket(bpy.types.NodeSocket):
    bl_idname = 'PH_MATERIAL_NODE_SOCKET'
    bl_label = "Photon Socket"

    link_only: bpy.props.BoolProperty(
        name="Link Only",
        description="Makes this node for linking only, its contained value(s) is ignored.",
        default=False
    )

    def draw_color(self, b_context, node):
        """
        Blender: Draw socket's color.
        """
        return [0.0, 0.0, 0.0, 1.0]

    def draw(self, b_context, b_layout, node, text):
        """
        Blender: Draw socket.
        """
        if not self.link_only:
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
        """
        Get the previously created SDL resource name from an input link.
        """
        if not self.links:
            return None

        from_socket = self.links[link_index].from_socket
        return naming.get_mangled_output_node_socket_name(from_socket, b_material)


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
    """
    General float value.
    """
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
    """
    Float value in the range [0, 1].
    """
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
class PhFloatVectorSocket(PhMaterialNodeSocket):
    """
    General float vector.
    """
    bl_idname = 'PH_FLOAT_VECTOR_SOCKET'
    bl_label = "Vector"

    default_value: bpy.props.FloatVectorProperty(
        name="Vector",
        default=[0, 0, 0],
        min=-1e32,
        max=1e32,
        subtype='NONE',
        size=3
    )

    def draw_color(self, b_context, node):
        return [0.476, 0.727, 0.829, 1.0]  # blue


@blender.register_class
class PhColorSocket(PhMaterialNodeSocket):
    """
    General color value.
    """
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
class PhColorSocketWithFloatDefault(PhMaterialNodeSocket):
    """
    General color value. This variant uses float as default value.
    """
    bl_idname = 'PH_COLOR_F_SOCKET'
    bl_label = "Color"

    default_value: bpy.props.FloatProperty(
        name="Float",
        default=0.5,
        min=-1e32,
        max=1e32,
        subtype='NONE'
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


class NodeCategory:
    def __init__(self, id_name, label):
        self.id_name = id_name
        self.label = label

    def __hash__(self):
        return hash((self.id_name, self.label))

    def __eq__(self, other):
        return (self.id_name, self.label) == (other.id_name, other.label)


OUTPUT_CATEGORY = NodeCategory("OUTPUT", "Output")
INPUT_CATEGORY = NodeCategory("INPUT", "Input")
SURFACE_MATERIAL_CATEGORY = NodeCategory("SURFACE", "Surface Material")
MATH_CATEGORY = NodeCategory("MATH", "Math")


@blender.register_class
class PhMaterialNodeTree(bpy.types.NodeTree):
    bl_idname = 'PH_MATERIAL_NODE_TREE'
    bl_label = "Photon Node Tree"
    bl_icon = 'MATERIAL'

    @classmethod
    def poll(cls, b_context):
        render_settings = b_context.scene.render
        return render_settings.engine in settings.photon_engines

    # Blender: set the current node tree to the one the active material owns (update editor views)
    @classmethod
    def get_from_context(cls, b_context):
        b_material = material.find_active_material_from_context(b_context)
        b_node_tree = material.find_node_tree_from_material(b_material)
        if b_material is not None and b_node_tree is not None:
            return b_node_tree, b_material, b_material

        return None, None, None


class PhMaterialNode(bpy.types.Node):
    bl_idname = 'PH_MATERIAL_NODE'
    bl_label = "Photon Node"
    bl_icon = 'MATERIAL'
    node_category = None

    def to_sdl(self, b_material, sdlconsole):
        raise NotImplementedError("to_sdl() must be implemented to support SDL generation")

    @classmethod
    def poll(cls, b_node_tree):
        """
        Blender: If non-null output is returned, the node type can be added to the tree.
        """
        return super().poll(b_node_tree) and b_node_tree.bl_idname == PhMaterialNodeTree.bl_idname

    def init(self, b_context):
        """
        Blender: Initialize a new instance of this node. Called when node created.
        """
        super().init(b_context)

    def draw_buttons(self, b_context, b_layout):
        """
        Blender: Draw node buttons. Draw properties in node.
        """
        pass

    def draw_label(self):
        """
        Blender: Returns a dynamic label string.
        """
        return self.bl_label


class PhMaterialOutputNode(PhMaterialNode):
    node_category = OUTPUT_CATEGORY


class PhMaterialInputNode(PhMaterialNode):
    node_category = INPUT_CATEGORY


class PhSurfaceMaterialNode(PhMaterialNode):
    node_category = SURFACE_MATERIAL_CATEGORY


class PhMaterialMathNode(PhMaterialNode):
    node_category = MATH_CATEGORY



