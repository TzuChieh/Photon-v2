"""
Basic definitions and data for node-based materials and node editor.
"""

from utility import settings, blender, material
from bmodule import naming
from psdl import sdl

import bpy
import nodeitems_utils
import sys


def get_material_resource_name(b_material):
    """
    Get the SDL resource name for a Blender material.
    """
    return naming.get_mangled_material_name(b_material)


class PhMaterialNodeSocket(bpy.types.NodeSocket):
    bl_idname = 'PH_MATERIAL_NODE_SOCKET'
    bl_label = "Photon Socket"

    link_only: bpy.props.BoolProperty(
        name="Link Only",
        description="Makes this node for linking only, its contained value(s) is ignored.",
        default=False,
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
        @return The resource name. `None` if not linked.
        """
        if not self.links:
            return None

        from_socket = self.links[link_index].from_socket
        return naming.get_mangled_output_node_socket_name(from_socket, b_material) if from_socket is not None else None


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
        size=3,
        )

    def draw_color(self, b_context, node):
        return [0.8, 0.1, 0.1, 1.0]  # red
    

@blender.register_class
class PhVolumeMaterialSocket(PhMaterialNodeSocket):
    bl_idname = 'PH_VOLUME_MATERIAL_SOCKET'
    bl_label = "Volume Material"

    def draw_color(self, b_context, node):
        return [0.1, 0.8, 0.1, 1.0]  # green


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
        subtype='NONE',
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
        subtype='FACTOR',
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
        size=3,
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
        size=3,
        )

    def draw_color(self, b_context, node):
        return [0.7, 0.7, 0.1, 1.0]  # yellow


@blender.register_class
class PhIorNSocket(PhMaterialNodeSocket):
    """
    Real part of a complex index of refraction.
    """
    bl_idname = 'PH_IOR_N_SOCKET'
    bl_label = "IoR N"

    default_value: bpy.props.FloatVectorProperty(
        name="IoR N",
        description="Vector values of IoR N.",
        default=[1.5, 1.5, 1.5],
        min=0.0,
        max=sys.float_info.max,
        subtype='COLOR',
        size=3,
        )

    def draw_color(self, b_context, node):
        return [0.7, 0.7, 0.1, 1.0]  # yellow


@blender.register_class
class PhIorKSocket(PhMaterialNodeSocket):
    """
    Imaginary part of a complex index of refraction.
    """
    bl_idname = 'PH_IOR_K_SOCKET'
    bl_label = "IoR K"

    default_value: bpy.props.FloatVectorProperty(
        name="IoR K",
        description="Vector values of IoR K.",
        default=[0.0, 0.0, 0.0],
        min=0.0,
        max=sys.float_info.max,
        subtype='COLOR',
        size=3,
        )

    def draw_color(self, b_context, node):
        return [0.7, 0.7, 0.1, 1.0]  # yellow


@blender.register_class
class PhLayerThicknessSocket(PhMaterialNodeSocket):
    """
    Physical thickness of a layer.
    """
    bl_idname = 'PH_LAYER_THICKNESS_SOCKET'
    bl_label = "Layer Thickness"

    default_value: bpy.props.FloatProperty(
        name="Layer Thickness",
        description="Physical thickness of a layer.",
        default=0.0,
        min=0.0,
        max=sys.float_info.max,
        subtype='NONE',
        )

    def draw_color(self, b_context, node):
        return [0.5, 0.5, 0.5, 1.0]  # gray


@blender.register_class
class PhHenyeyGreensteinGSocket(PhMaterialNodeSocket):
    """
    The g variable in the Henyey-Greenstein phase function.
    """
    bl_idname = 'PH_HENYEY_GREENSTEIN_G_SOCKET'
    bl_label = "G"

    default_value: bpy.props.FloatProperty(
        name="G",
        default=0.9,
        min=0.5,
        max=1.0,
        subtype='NONE',
        )

    def draw_color(self, b_context, node):
        return [0.5, 0.5, 0.5, 1.0]  # gray


@blender.register_class
class PhVolumeAbsorptionSocket(PhMaterialNodeSocket):
    """
    Volume absorption coefficient.
    """
    bl_idname = 'PH_VOLUME_ABSORPTION_SOCKET'
    bl_label = "Sigma A"

    default_value: bpy.props.FloatVectorProperty(
        name="Sigma A",
        description="Volume absorption coefficient.",
        default=[0.1, 0.1, 0.1],
        min=0.0,
        max=sys.float_info.max,
        subtype='COLOR',
        size=3,
        )

    def draw_color(self, b_context, node):
        return [0.7, 0.7, 0.1, 1.0]  # yellow


@blender.register_class
class PhVolumeScatteringSocket(PhMaterialNodeSocket):
    """
    Volume scattering coefficient.
    """
    bl_idname = 'PH_VOLUME_SCATTERING_SOCKET'
    bl_label = "Sigma S"

    default_value: bpy.props.FloatVectorProperty(
        name="Sigma S",
        description="Volume scattering coefficient.",
        default=[0.1, 0.1, 0.1],
        min=0.0,
        max=sys.float_info.max,
        subtype='COLOR',
        size=3,
        )

    def draw_color(self, b_context, node):
        return [0.7, 0.7, 0.1, 1.0]  # yellow


@blender.register_class
class PhF0Socket(PhMaterialNodeSocket):
    """
    Surface reflectance at normal incidence.
    """
    bl_idname = 'PH_F0_SOCKET'
    bl_label = "F0"

    default_value: bpy.props.FloatVectorProperty(
        name="F0",
        description="F0 value",
        default=[0.9, 0.9, 0.9],
        min=0.0,
        max=1.0,
        subtype='COLOR',
        size=3,
        )

    def draw_color(self, b_context, node):
        return [0.7, 0.7, 0.1, 1.0]  # yellow


@blender.register_class
class PhReflectionScaleSocket(PhMaterialNodeSocket):
    """
    Reflection intensity scaling factor for artistic control.
    """
    bl_idname = 'PH_REFLECTION_SCALE_SOCKET'
    bl_label = "Reflection Scale"

    default_value: bpy.props.FloatVectorProperty(
        name="Reflection Scale",
        description="Reflection intensity scaling factor for artistic control.",
        default=[1.0, 1.0, 1.0],
        min=-sys.float_info.max,
        max=sys.float_info.max,
        subtype='COLOR',
        size=3,
        )

    def draw_color(self, b_context, node):
        return [0.7, 0.7, 0.1, 1.0]  # yellow


@blender.register_class
class PhTransmissionScaleSocket(PhMaterialNodeSocket):
    """
    Transmission intensity scaling factor for artistic control.
    """
    bl_idname = 'PH_TRANSMISSION_SCALE_SOCKET'
    bl_label = "Transmission Scale"

    default_value: bpy.props.FloatVectorProperty(
        name="Transmission Scale",
        description="Transmission intensity scaling factor for artistic control.",
        default=[1.0, 1.0, 1.0],
        min=-sys.float_info.max,
        max=sys.float_info.max,
        subtype='COLOR',
        size=3,
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
        subtype='NONE',
        )

    def draw_color(self, b_context, node):
        return [0.7, 0.7, 0.1, 1.0]  # yellow


@blender.register_class
class PhSurfaceLayerSocket(PhMaterialNodeSocket):
    bl_idname = 'PH_SURFACE_LAYER_SOCKET'
    bl_label = "Surface Layer"

    def draw_color(self, b_context, node):
        return [0.0, 0.0, 0.0, 1.0]  # black


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


@blender.register_class
class PH_MATERIAL_OT_make_node_group(bpy.types.Operator):
    bl_label = "Make Group"
    bl_idname = "photon.make_node_group"

    @classmethod
    def poll(cls, b_context):
        b_material = getattr(b_context, 'material', None)
        return b_material is not None and b_material.photon.node_tree is not None

    def execute(self, b_context):
        bpy.data.node_groups.new("Photon Node Group", PhMaterialNodeTree.bl_idname)
        
        return {'FINISHED'}
    

# TODO: add_node operator
        

class NodeCategory:
    def __init__(self, id_name, label):
        self.id_name = id_name
        self.label = label

    def __hash__(self):
        return hash((self.id_name, self.label))

    def __eq__(self, other):
        return (self.id_name, self.label) == (other.id_name, other.label)


@blender.register_class
class PhMaterialNodeTree(bpy.types.NodeTree):
    bl_idname = 'PH_MATERIAL_NODE_TREE'
    bl_label = "Photon Node Tree"
    bl_icon = 'MATERIAL'

    associated_node_idname: bpy.props.StringProperty(
        name="",
        description="The bl_idname of the node that associated to this tree.",
        default="",
        options={'ANIMATABLE', 'HIDDEN'}
    )

    @classmethod
    def poll(cls, b_context):
        render_settings = b_context.scene.render
        return render_settings.engine in settings.photon_engines

    @classmethod
    def get_from_context(cls, b_context):
        """
        Blender: Set the current node tree to the one the active material owns (update editor views).
        """
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

    def get_linked_input_resource_name(self, b_material, input_index, link_index=0):
        """
        Get the SDL resource name provided by a linked input socket.
        """
        return self.inputs[input_index].get_from_res_name(b_material, link_index)

    def get_default_input_resource_name(self, b_material, input_index):
        """
        Get the SDL resource name for this node's unlinked input value.
        """
        return naming.get_mangled_input_node_socket_name(self.inputs[input_index], b_material)

    def get_default_input_value(self, input_index):
        """
        Get this node's default socket value for an unlinked input.
        """
        return self.inputs[input_index].default_value

    def get_output_resource_name(self, b_material, output_index=0):
        """
        Get the SDL resource name created for this node's output socket.
        """
        return naming.get_mangled_output_node_socket_name(self.outputs[output_index], b_material)

    def get_node_resource_name(self, b_material, suffix=None):
        """
        Get this node's SDL resource name, optionally for a node-owned helper resource.
        """
        if suffix is None:
            return naming.get_mangled_node_name(self, b_material)

        return naming.get_mangled_node_name(self, b_material, suffix=suffix)

    def get_material_resource_name(self, b_material):
        """
        Get the SDL resource name for the owning Blender material.
        """
        return get_material_resource_name(b_material)

    def warn_incomplete_node(self, b_material, message=None):
        warning = f"warning: material <{b_material.name}>'s {self.bl_label} node is incomplete"
        if message:
            warning = f"{warning}: {message}"
        print(warning)

    def queue_fallback_material(self, sdlconsole, resource_name):
        """
        Queue a valid fallback material with the specified SDL resource name.
        """
        creator = sdl.MatteOpaqueMaterialCreator()
        creator.set_data_name(resource_name)
        sdlconsole.queue_command(creator)

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
    node_category = NodeCategory('OUTPUT', "Output")


class PhMaterialInputNode(PhMaterialNode):
    node_category = NodeCategory('INPUT', "Input")


class PhSurfaceMaterialNode(PhMaterialNode):
    node_category = NodeCategory('SURFACE', "Surface Material")


class PhVolumeMaterialNode(PhMaterialNode):
    node_category = NodeCategory('VOLUME', "Volume Material")


class PhMaterialMathNode(PhMaterialNode):
    node_category = NodeCategory('MATH', "Math")


class PhMaterialGroupNode(PhMaterialNode):
    node_category = NodeCategory('GROUP', "Group")
