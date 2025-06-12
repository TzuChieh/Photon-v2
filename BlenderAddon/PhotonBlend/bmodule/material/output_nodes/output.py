from ..node_base import (
    PhMaterialOutputNode,
    PhSurfaceMaterialSocket,
    PhVolumeMaterialSocket,
    PhColorSocket,
    PhFloatFactorSocket,
    )
from psdl import sdl
from bmodule import naming


class PhOutputNode(PhMaterialOutputNode):
    bl_idname = 'PH_OUTPUT'
    bl_label = "Output"

    def to_sdl(self, b_material, sdlconsole):
        surface_mat_socket = self.inputs[0]
        surface_mat_res_name = surface_mat_socket.get_from_res_name(b_material)

        volume_mat_socket = self.inputs[1]
        volume_mat_res_name = volume_mat_socket.get_from_res_name(b_material)

        if not surface_mat_res_name and not volume_mat_res_name:
            if not self.get_surface_emission_res_name():
                print(f"material {b_material.name}'s primary output nodes are not linked, ignoring")
            return

        assert surface_mat_res_name or volume_mat_res_name

        creator = sdl.FullMaterialCreator()
        creator.set_data_name(naming.get_mangled_material_name(b_material))
        if surface_mat_res_name:
            creator.set_surface(sdl.Material(surface_mat_res_name))
        if volume_mat_res_name:
            creator.set_interior(sdl.Material(volume_mat_res_name))
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.inputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)
        self.inputs[0].link_only = True

        self.inputs.new(PhVolumeMaterialSocket.bl_idname, PhVolumeMaterialSocket.bl_label)
        self.inputs[1].link_only = True

        self.inputs.new(PhColorSocket.bl_idname, "Surface Emission")
        self.inputs[2].link_only = True

        self.inputs.new(PhFloatFactorSocket.bl_idname, "Surface Mask")
        self.inputs[3].link_only = True

    def get_surface_emission_res_name(self, b_material):
        surface_emission_socket = self.inputs[2]
        return surface_emission_socket.get_from_res_name(b_material)
    
    def get_surface_mask_res_name(self, b_material):
        surface_mask_socket = self.inputs[3]
        return surface_mask_socket.get_from_res_name(b_material)
