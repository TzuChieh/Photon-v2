from ..node_base import (
        PhMaterialInputNode,
        PhFloatValueSocket)
from psdl import sdl

import bpy


class PhFloatValueInputNode(PhMaterialInputNode):
    bl_idname = 'PH_FLOAT_VALUE'
    bl_label = "Float Value"

    value: bpy.props.FloatProperty(
        name="Value",
        default=1.0,
        min=-1e32,
        max=1e32
    )

    def to_sdl(self, b_material, sdlconsole):
        creator = sdl.ConstantImageCreator()
        creator.set_data_name(self.get_output_resource_name(b_material))
        creator.set_values(sdl.RealArray([self.value]))
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.outputs.new(PhFloatValueSocket.bl_idname, PhFloatValueSocket.bl_label)

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, 'value')
