from ..node_base import (
        PhSurfaceMaterialNode,
        PhSurfaceMaterialSocket,
        PhColorSocket)
from psdl import sdl

import bpy
import mathutils


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
        albedo_img_name = self.get_linked_input_resource_name(b_material, 0)
        if albedo_img_name is None:
            albedo_img_name = self.get_default_input_resource_name(b_material, 0)
            albedo_img = sdl.ConstantImageCreator()
            albedo_img.set_data_name(albedo_img_name)
            albedo_img.set_values(sdl.RealArray(self.get_default_input_value(0)))
            albedo_img.set_color_space(sdl.Enum('LSRGB'))
            sdlconsole.queue_command(albedo_img)

        sigma_img_name = None
        if self.diffusion_type == 'OREN_NAYAR':
            sigma_img_name = self.get_node_resource_name(b_material, suffix="sigma")
            sigma_img = sdl.ConstantImageCreator()
            sigma_img.set_data_name(sigma_img_name)
            sigma_img.set_values(sdl.RealArray([self.roughness * 180.0]))
            sdlconsole.queue_command(sigma_img)

        creator = sdl.MatteOpaqueMaterialCreator()
        creator.set_data_name(self.get_output_resource_name(b_material))
        creator.set_albedo(sdl.Image(albedo_img_name))

        if sigma_img_name:
            creator.set_sigma_degrees(sdl.Image(sigma_img_name))

        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.inputs.new(PhColorSocket.bl_idname, "Albedo")
        self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)

    def draw_buttons(self, b_context, b_layout):
        row = b_layout.row()
        row.prop(self, 'diffusion_type', text="")

        if self.diffusion_type == 'OREN_NAYAR':
            row = b_layout.row()
            row.prop(self, 'roughness')
