from ..node_base import (
        PhSurfaceMaterialNode,
        PhSurfaceMaterialSocket,
        PhFloatFactorSocket,
        PhColorSocket)
from psdl import sdl

import bpy


class PhDiffuseSurfaceNode(PhSurfaceMaterialNode):
    bl_idname = 'PH_DIFFUSE_SURFACE'
    bl_label = "Diffuse Surface"

    diffusion_type: bpy.props.EnumProperty(
        items=[
            ('LAMBERTIAN', "Lambertian", "", 0),
            ('OREN_NAYAR', "Oren Nayar", "", 1)
        ],
        name="Type",
        description="surface diffusion types",
        default='LAMBERTIAN',
        )

    roughness: bpy.props.FloatProperty(
        name="Roughness",
        default=0.5,
        min=0.0,
        max=1.0,
        )

    def to_sdl(self, b_material, sdlconsole):
        creator = sdl.MatteOpaqueMaterialCreator()
        creator.set_data_name(self.get_output_resource_name(b_material))

        albedo_img_name = self.get_linked_input_resource_name(b_material, 0)
        if albedo_img_name is not None:
            creator.set_albedo_map(sdl.Image(albedo_img_name))
        else:
            creator.set_albedo(sdl.Spectrum(self.get_default_input_value(0)))

        if self.diffusion_type == 'OREN_NAYAR':
            roughness_img_name = self.get_linked_input_resource_name(b_material, 1)
            if roughness_img_name is not None:
                creator.set_sigma_map(sdl.Image(roughness_img_name))
            else:
                creator.set_sigma(sdl.Real(self.roughness))

        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.inputs.new(PhColorSocket.bl_idname, "Albedo")

        roughness_socket = self.inputs.new(PhFloatFactorSocket.bl_idname, "Roughness")
        roughness_socket.link_only = True
        
        self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)

    def draw_buttons(self, b_context, b_layout):
        row = b_layout.row()
        row.prop(self, 'diffusion_type', text="")

        if self.diffusion_type == 'OREN_NAYAR':
            row = b_layout.row()
            row.prop(self, 'roughness')
