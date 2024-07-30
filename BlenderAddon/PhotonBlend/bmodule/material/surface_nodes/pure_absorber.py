from ..node_base import (
        PhSurfaceMaterialNode,
        PhSurfaceMaterialSocket,
        )
from psdl import sdl
from bmodule import naming


class PhPureAbsorberNode(PhSurfaceMaterialNode):
    bl_idname = 'PH_PURE_ABSORBER'
    bl_label = "Pure Absorber"

    def to_sdl(self, b_material, sdlconsole):
        surface_mat_socket = self.outputs[0]
        surface_mat_res_name = naming.get_mangled_output_node_socket_name(surface_mat_socket, b_material)

        creator = sdl.IdealSubstanceMaterialCreator()
        creator.set_data_name(surface_mat_res_name)
        creator.set_substance(sdl.Enum("absorber"))
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)
