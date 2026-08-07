from ..node_base import (
    PhMaterialOutputNode,
    PhSurfaceMaterialSocket,
    PhVolumeMaterialSocket,
    PhColorSocket,
    PhFloatFactorSocket,
    queue_fallback_material,
    )
from bmodule import naming
from psdl import sdl


class PhOutputNode(PhMaterialOutputNode):
    bl_idname = 'PH_OUTPUT'
    bl_label = "Output"

    def to_sdl(self, b_material, sdlconsole):
        surface_mat_res_name = self.get_linked_input_resource_name(b_material, 0)
        volume_mat_res_name = self.get_linked_input_resource_name(b_material, 1)
        interface_mask_res_name = self.get_interface_mask_res_name(b_material)

        if not surface_mat_res_name and not volume_mat_res_name:
            if not self.get_surface_emission_res_name(b_material):
                self.warn_incomplete_node(b_material, "surface and volume inputs are not linked")
            queue_fallback_material(
                b_material,
                sdlconsole,
                interface_mask_res_name=interface_mask_res_name)
            return

        assert surface_mat_res_name or volume_mat_res_name

        creator = sdl.FullMaterialCreator()
        creator.set_data_name(naming.get_mangled_material_name(b_material))
        creator.set_display_name(sdl.String(b_material.name))
        if surface_mat_res_name:
            creator.set_surface(sdl.Material(surface_mat_res_name))
        if volume_mat_res_name:
            creator.set_interior(sdl.Material(volume_mat_res_name))
        if interface_mask_res_name:
            creator.set_interface_mask(sdl.Image(interface_mask_res_name))
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.inputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)
        self.inputs[0].link_only = True

        self.inputs.new(PhVolumeMaterialSocket.bl_idname, PhVolumeMaterialSocket.bl_label)
        self.inputs[1].link_only = True

        self.inputs.new(PhColorSocket.bl_idname, "Surface Emission")
        self.inputs[2].link_only = True

        self.inputs.new(PhFloatFactorSocket.bl_idname, "Interface Mask")
        self.inputs[3].link_only = True

    def get_surface_emission_res_name(self, b_material):
        return self.get_linked_input_resource_name(b_material, 2)
    
    def get_interface_mask_res_name(self, b_material):
        return self.get_linked_input_resource_name(b_material, 3)
