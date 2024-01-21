from ..node_base import (
        PhMaterialInputNode,
        PhColorSocket)
from psdl import sdl
from ... import naming

import bpy
import mathutils


class PhConstantColorInputNode(PhMaterialInputNode):
    bl_idname = 'PH_CONSTANT_COLOR'
    bl_label = "Constant Color"

    color: bpy.props.FloatVectorProperty(
        name="Color",
        description="color value",
        default=[0.5, 0.5, 0.5],
        min=0.0,
        max=1.0,
        subtype='COLOR',
        size=3
    )

    # TODO: color space
    # usage: bpy.props.EnumProperty(
    #     items=[
    #         ('EMISSION', "Emission", "", 0),
    #         ('REFLECTANCE', "Reflectance", "", 1)
    #     ],
    #     name="Usage",
    #     description="What is the color for",
    #     default='REFLECTANCE'
    # )

    def to_sdl(self, b_material, sdlconsole):
        output_socket = self.outputs[0]
        creator = sdl.ConstantImageCreator()
        creator.set_data_name(naming.get_mangled_output_node_socket_name(output_socket, b_material))
        creator.set_values(sdl.RealArray(mathutils.Color((self.color[0], self.color[1], self.color[2]))))
        creator.set_color_space(sdl.Enum("LSRGB"))
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.outputs.new(PhColorSocket.bl_idname, PhColorSocket.bl_label)

    def draw_buttons(self, b_context, b_layout):
        b_layout.template_color_picker(self, 'color', value_slider=True)
        b_layout.prop(self, 'color', text="")
        # b_layout.prop(self, "usage", text="")
