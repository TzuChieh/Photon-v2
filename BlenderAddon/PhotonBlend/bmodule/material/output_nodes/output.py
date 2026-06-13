from ..node_base import (
    PhMaterialOutputNode,
    PhSurfaceMaterialSocket,
    PhVolumeMaterialSocket,
    PhColorSocket,
    PhFloatFactorSocket,
    )
from psdl import sdl


class PhOutputNode(PhMaterialOutputNode):
    bl_idname = 'PH_OUTPUT'
    bl_label = "Output"

    def to_sdl(self, b_material, sdlconsole):
        surface_mat_res_name = self.get_linked_input_resource_name(b_material, 0)
        volume_mat_res_name = self.get_linked_input_resource_name(b_material, 1)

        if not surface_mat_res_name and not volume_mat_res_name:
            if not self.get_surface_emission_res_name(b_material):
                self.warn_incomplete_node(b_material, "surface and volume inputs are not linked")
            self.queue_fallback_material(sdlconsole, self.get_material_resource_name(b_material))
            return

        assert surface_mat_res_name or volume_mat_res_name

        creator = sdl.FullMaterialCreator()
        creator.set_data_name(self.get_material_resource_name(b_material))
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
        return self.get_linked_input_resource_name(b_material, 2)
    
    def get_surface_mask_res_name(self, b_material):
        return self.get_linked_input_resource_name(b_material, 3)
