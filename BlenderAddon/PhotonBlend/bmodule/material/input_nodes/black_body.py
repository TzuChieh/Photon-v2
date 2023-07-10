from ..node_base import (
        PhMaterialInputNode,
        PhColorSocket)
from psdl import sdl
from ... import naming

import bpy


class PhBlackBodyInputNode(PhMaterialInputNode):
    bl_idname = 'PH_BLACK_BODY'
    bl_label = "Black Body"

    kelvin: bpy.props.FloatProperty(
        name="Kelvin",
        description="Temperature in Kelvin.",
        default=3000.0,
        min=0,
        max=1e32
    )

    energy: bpy.props.FloatProperty(
        name="Energy",
        description="The amount of energy the black body emits.",
        default=1.0,
        min=0,
        max=1e32
    )

    def to_sdl(self, b_material, sdlconsole):
        output_socket = self.outputs[0]
        creator = sdl.BlackBodyImageCreator()
        creator.set_data_name(naming.get_mangled_output_node_socket_name(output_socket, b_material))
        creator.set_temperature_k(sdl.Real(self.kelvin))
        creator.set_energy(sdl.Real(self.energy))
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.outputs.new(PhColorSocket.bl_idname, PhColorSocket.bl_label)

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, 'kelvin')
        b_layout.prop(self, 'energy')
