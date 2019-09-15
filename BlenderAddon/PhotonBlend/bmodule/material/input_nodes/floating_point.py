from ..node_base import (
        PhMaterialNode,
        PhFloatValueSocket,
        INPUT_CATEGORY)
from ....psdl.pysdl import (
        ConstantImageCreator,
        SDLReal,
        SDLString)
from ... import naming

import bpy


class PhFloatValueInputNode(PhMaterialNode):
    bl_idname = 'PH_FLOAT_VALUE'
    bl_label = "Float Value"
    node_category = INPUT_CATEGORY

    value: bpy.props.FloatProperty(
        name="Value",
        default=1.0,
        min=-1e32,
        max=1e32
    )

    usage: bpy.props.EnumProperty(
        items=[
            ('RAW', "Raw", ""),
            ('EMISSION', "Emission", ""),
            ('REFLECTANCE', "Reflectance", "")
        ],
        name="Usage",
        description="What is the value for",
        default='RAW'
    )

    def to_sdl(self, b_material, sdlconsole):
        output_socket = self.outputs[0]
        creator = ConstantImageCreator()
        creator.set_data_name(naming.get_mangled_output_node_socket_name(output_socket, b_material))
        creator.set_value(SDLReal(self.value))
        if self.usage == 'RAW':
            creator.set_value_type(SDLString("raw"))
        elif self.usage == 'EMISSION':
            creator.set_value_type(SDLString("emr-linear-srgb"))
        elif self.usage == 'REFLECTANCE':
            creator.set_value_type(SDLString("ecf-linear-srgb"))
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.outputs.new(PhFloatValueSocket.bl_idname, PhFloatValueSocket.bl_label)

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, 'value')
        b_layout.prop(self, 'usage', text="")
