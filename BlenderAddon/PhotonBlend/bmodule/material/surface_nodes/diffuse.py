from ..node_base import (
        PhSurfaceMaterialNode,
        PhSurfaceMaterialSocket,
        PhColorSocket)
from psdl import sdl
from ... import naming

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
        default='LAMBERTIAN'
    )

    roughness: bpy.props.FloatProperty(
        name="Roughness",
        default=0.5,
        min=0.0,
        max=1.0
    )

    def to_sdl(self, b_material, sdlconsole):
        albedo_socket = self.inputs[0]
        surface_material_socket = self.outputs[0]

        albedo_res_name = albedo_socket.get_from_res_name(b_material)
        if albedo_res_name is None:
            creator = sdl.ConstantImageCreator()
            albedo_res_name = naming.get_mangled_input_node_socket_name(albedo_socket, b_material)
            creator.set_data_name(albedo_res_name)
            albedo = albedo_socket.default_value
            creator.set_values(sdl.RealArray(mathutils.Color((albedo[0], albedo[1], albedo[2]))))
            creator.set_color_space(sdl.Enum("LSRGB"))
            sdlconsole.queue_command(creator)

        creator = sdl.MatteOpaqueMaterialCreator()
        creator.set_data_name(naming.get_mangled_output_node_socket_name(surface_material_socket, b_material))
        creator.set_albedo(sdl.Image(albedo_res_name))
        if self.diffusion_type == 'OREN_NAYAR':
            creator.set_sigma_degrees(sdl.Real(self.roughness * 180.0))
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
