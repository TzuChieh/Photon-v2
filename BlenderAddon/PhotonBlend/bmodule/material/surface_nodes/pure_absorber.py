from ..node_base import (
        PhSurfaceMaterialNode,
        PhSurfaceMaterialSocket,
        )
from psdl import sdl


class PhPureAbsorberNode(PhSurfaceMaterialNode):
    bl_idname = 'PH_PURE_ABSORBER'
    bl_label = "Pure Absorber"

    def to_sdl(self, b_material, sdlconsole):
        creator = sdl.IdealSubstanceMaterialCreator()
        creator.set_data_name(self.get_output_resource_name(b_material))
        creator.set_substance(sdl.Enum("absorber"))
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)
