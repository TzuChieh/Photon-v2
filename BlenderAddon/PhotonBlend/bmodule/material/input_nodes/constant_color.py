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
        description="Color value.",
        default=[0.5, 0.5, 0.5],
        min=0.0,
        max=1.0,
        subtype='COLOR',
        size=3
    )

    color_space: bpy.props.EnumProperty(
        items=[
            ('auto', "Auto", "Use the scene linear color space of Blender.", 0),
            ('XYZ', "CIE XYZ", "", 1),
            ('xyY', "CIE xyY", "", 2),
            ('LSRGB', "Linear sRGB", "", 3),
            ('SRGB', "sRGB", "", 4),
            ('ACEScg', "ACEScg", "", 5),
        ],
        name="Color Space",
        description="Color space of the constant.",
        default='auto'
    )

    is_raw_data: bpy.props.BoolProperty(
        name="Raw Data",
        description="Use the color as raw data. No color space transformation will be done during rendering.",
        default=False
    )

    def to_sdl(self, b_material, sdlconsole):
        output_socket = self.outputs[0]
        creator = sdl.ConstantImageCreator()
        creator.set_data_name(naming.get_mangled_output_node_socket_name(output_socket, b_material))
        creator.set_values(sdl.RealArray(mathutils.Color((self.color[0], self.color[1], self.color[2]))))
        
        if self.color_space == 'auto':
            # FIXME: properly obtain the scene linear color space Blender uses, how to obtain? access to OCIO config?
            creator.set_color_space(sdl.Enum('LSRGB'))
        elif not self.is_raw_data:
            creator.set_color_space(sdl.Enum(self.color_space))

        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.outputs.new(PhColorSocket.bl_idname, PhColorSocket.bl_label)

    def draw_buttons(self, b_context, b_layout):
        # Show a color picker. This is always in Blender's scene linear color space.
        b_layout.template_color_picker(self, 'color', value_slider=True)
        
        b_layout.prop(self, 'color', text="")
        b_layout.prop(self, 'is_raw_data')

        b_sub_col = b_layout.column(align=True)
        b_sub_col.enabled = not self.is_raw_data
        b_sub_col.prop(self, 'color_space', text="Color")
