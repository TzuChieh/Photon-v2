from ..node_base import (
    PhSurfaceMaterialNode,
    PhSurfaceMaterialSocket,
    PhColorSocket)
from psdl import sdl


class PhNormalMappedSurfaceNode(PhSurfaceMaterialNode):
    bl_idname = 'PH_NORMAL_MAPPED_SURFACE'
    bl_label = "Normal Mapped Surface"

    def to_sdl(self, b_material, sdlconsole):
        material_res_name = self.get_linked_input_resource_name(b_material, 0)
        normal_map_res_name = self.get_linked_input_resource_name(b_material, 1)
        if material_res_name is None or normal_map_res_name is None:
            self.warn_incomplete_node(b_material, "surface material or normal map input is not linked")
            self.queue_fallback_material(sdlconsole, self.get_output_resource_name(b_material))
            return

        creator = sdl.SurfaceNormalMapMaterialCreator()
        creator.set_data_name(self.get_output_resource_name(b_material))
        creator.set_material(sdl.Material(material_res_name))
        creator.set_map(sdl.Image(normal_map_res_name))
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.inputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)
        self.inputs[0].link_only = True

        self.inputs.new(PhColorSocket.bl_idname, "Normal Map")
        self.inputs[1].link_only = True

        self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)
