from ..node_base import (
    PhMaterialConversionNode,
    PhColorSocket)
from psdl import sdl

import bpy
from mathutils import Euler


class PhTransformedImageNode(PhMaterialConversionNode):
    bl_idname = 'PH_TRANSFORMED_IMAGE'
    bl_label = "Image Transform"
    bl_width_default = 220

    translation: bpy.props.FloatVectorProperty(
        name="Translation",
        description="Translation applied to the image coordinates.",
        default=(0.0, 0.0, 0.0),
        subtype='TRANSLATION',
        size=3)

    rotation: bpy.props.FloatVectorProperty(
        name="Rotation",
        description="XYZ Euler rotation applied to the image coordinates.",
        default=(0.0, 0.0, 0.0),
        subtype='EULER',
        unit='ROTATION',
        size=3)

    scale: bpy.props.FloatVectorProperty(
        name="Scale",
        description="Scale applied to the image coordinates.",
        default=(1.0, 1.0, 1.0),
        subtype='XYZ',
        size=3)

    def to_sdl(self, b_material, sdlconsole):
        if not self.outputs[0].is_linked:
            return

        output_res_name = self.get_output_resource_name(b_material)
        input_res_name = self.get_linked_input_resource_name(b_material, 0)
        if input_res_name is None:
            self.warn_incomplete_node(b_material, "image input is not linked")
            fallback = sdl.ConstantImageCreator()
            fallback.set_data_name(output_res_name)
            fallback.set_values(sdl.RealArray([0]))
            sdlconsole.queue_command(fallback)
            return

        rotation = Euler(self.rotation, 'XYZ').to_quaternion()
        creator = sdl.TransformedImageCreator()
        creator.set_data_name(output_res_name)
        creator.set_input(sdl.Image(input_res_name))
        creator.set_pos(sdl.Vector3(self.translation))
        creator.set_rot(sdl.Quaternion((rotation.x, rotation.y, rotation.z, rotation.w)))
        creator.set_scale(sdl.Vector3(self.scale))
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        image_input = self.inputs.new(PhColorSocket.bl_idname, "Image")
        image_input.link_only = True
        self.outputs.new(PhColorSocket.bl_idname, "Image")

    def draw_buttons(self, b_context, b_layout):
        transform_column = b_layout.column(align=True)
        transform_column.prop(self, 'translation')
        transform_column.prop(self, 'rotation')
        transform_column.prop(self, 'scale')
