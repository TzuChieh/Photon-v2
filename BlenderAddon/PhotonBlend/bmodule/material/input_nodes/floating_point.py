from ..node_base import (
        PhMaterialInputNode,
        PhFloatValueSocket)
from psdl import sdl
from ... import naming

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
        output_socket = self.outputs[0]
        creator = sdl.ConstantImageCreator()
        creator.set_data_name(naming.get_mangled_output_node_socket_name(output_socket, b_material))
        creator.set_values(sdl.RealArray([self.value]))
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.outputs.new(PhFloatValueSocket.bl_idname, PhFloatValueSocket.bl_label)

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, 'value')
