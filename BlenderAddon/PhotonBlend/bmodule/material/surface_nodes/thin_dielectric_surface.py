from ..node_base import (
    PhSurfaceMaterialNode,
    PhFloatValueSocket,
    PhColorSocketWithFloatDefault,
    PhColorSocket,
    PhSurfaceMaterialSocket,
    )
from psdl import sdl
from bmodule import naming

import bpy

import sys


class PhThinDielectricSurfaceNode(PhSurfaceMaterialNode):
    bl_idname = 'PH_THIN_DIELECTRIC_SURFACE'
    bl_label = "Thin Dielectric Surface"

    fresnel_type: bpy.props.EnumProperty(
        items=[
            ('schlick', "Schlick Approximation", "", 0),
            ('exact', "Exact", "", 1),
        ],
        name="Fresnel Type",
        description="Type of Fresnel effect used.",
        default='exact',
        )

    ior_outer: bpy.props.FloatProperty(
        name="IoR Outer",
        default=1.0,
        min=0.0,
        max=sys.float_info.max,
        )

    ior_inner: bpy.props.FloatProperty(
        name="IoR Inner",
        default=1.5,
        min=0.0,
        max=sys.float_info.max,
        )

    def to_sdl(self, b_material, sdlconsole):
        thickness_socket = self.inputs[0]
        sigma_t_socket = self.inputs[1]
        reflection_scale_socket = self.inputs[2]
        transmission_scale_socket = self.inputs[3]
        surface_material_socket = self.outputs[0]

        thickness_img_name = thickness_socket.get_from_res_name(b_material)
        if not thickness_img_name and thickness_socket.default_value > 0:
            thickness_img_name = naming.get_mangled_input_node_socket_name(thickness_socket, b_material)
            thickness_img = sdl.ConstantImageCreator()
            thickness_img.set_data_name(thickness_img_name)
            thickness_img.set_values(sdl.RealArray([thickness_socket.default_value]))
            sdlconsole.queue_command(thickness_img)

        material = sdl.ThinDielectricSurfaceMaterialCreator()
        material.set_data_name(naming.get_mangled_output_node_socket_name(surface_material_socket, b_material))

        material.set_fresnel(sdl.Enum(self.fresnel_type))
        material.set_ior_outer(sdl.Real(self.ior_outer))
        material.set_ior_inner(sdl.Real(self.ior_inner))

        if thickness_img_name:
            material.set_thickness(sdl.Image(thickness_img_name))

            sigma_t_img_name = sigma_t_socket.get_from_res_name(b_material)
            if not sigma_t_img_name:
                sigma_t_img_name = naming.get_mangled_input_node_socket_name(sigma_t_socket, b_material)
                sigma_t_img = sdl.ConstantImageCreator()
                sigma_t_img.set_data_name(sigma_t_img_name)
                sigma_t_img.set_values(sdl.RealArray([sigma_t_socket.default_value]))
                sdlconsole.queue_command(sigma_t_img)

            material.set_sigma_t(sdl.Image(sigma_t_img_name))
        
        reflection_scale_img_name = reflection_scale_socket.get_from_res_name(b_material)
        if reflection_scale_img_name:
            material.set_reflection_scale(sdl.Image(reflection_scale_img_name))

        transmission_scale_img_name = transmission_scale_socket.get_from_res_name(b_material)
        if transmission_scale_img_name:
            material.set_transmission_scale(sdl.Image(transmission_scale_img_name))

        sdlconsole.queue_command(material)

    def init(self, b_context):
        self.inputs.new(PhFloatValueSocket.bl_idname, "Thickness")
        self.inputs.new(PhColorSocketWithFloatDefault.bl_idname, "Sigma T")
        self.inputs.new(PhColorSocket.bl_idname, "Reflection Scale")
        self.inputs.new(PhColorSocket.bl_idname, "Transmission Scale")
        self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)

        self.inputs[0].default_value = 0.0
        self.inputs[1].default_value = 1.0
        self.inputs[2].link_only = True
        self.inputs[3].link_only = True

        self.width *= 1.2

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, 'fresnel_type', text="")
        b_layout.prop(self, 'ior_outer')
        b_layout.prop(self, 'ior_inner')
