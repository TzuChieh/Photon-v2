from ..node_base import (
    PhVolumeMaterialNode,
    PhVolumeMaterialSocket)
from psdl import sdl
from bmodule import naming

import bpy

import sys


class PhIdealMediumNode(PhVolumeMaterialNode):
    bl_idname = 'PH_IDEAL_MEDIUM'
    bl_label = "Ideal Medium"

    absorption_coeff: bpy.props.FloatVectorProperty(
        name="Absorption Coefficient",
        description=sdl.IdealMediumMaterialCreator.set_absorption_coeff.__doc__,
        default=[0.5, 0.5, 0.5],
        min=0.0,
        max=sys.float_info.max,
        subtype='COLOR',
        size=3
    )

    def to_sdl(self, b_material, sdlconsole):
        mat_socket = self.outputs[0]
        mat_res_name = naming.get_mangled_output_node_socket_name(mat_socket, b_material)

        creator = sdl.IdealMediumMaterialCreator()
        creator.set_data_name(mat_res_name)
        creator.set_absorption_coeff(sdl.Spectrum(self.absorption_coeff))

        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.outputs.new(PhVolumeMaterialSocket.bl_idname, PhVolumeMaterialSocket.bl_label)

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, 'absorption_coeff', text="")
