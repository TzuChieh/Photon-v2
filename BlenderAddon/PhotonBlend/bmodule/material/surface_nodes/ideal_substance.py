from ..node_base import (
        PhSurfaceMaterialNode,
        PhSurfaceMaterialSocket,
        PhF0Socket,
        PhReflectionScaleSocket,
        PhTransmissionScaleSocket)
from psdl import sdl

import bpy

import sys


class PhIdealSubstanceNode(PhSurfaceMaterialNode):
    bl_idname = 'PH_IDEAL_SUBSTANCE'
    bl_label = "Ideal Substance"

    substance_type: bpy.props.EnumProperty(
        items=[
            ('dielectric-reflector', "Dielectric Reflector", "", 0),
            ('dielectric-transmitter', "Dielectric Transmitter", "", 2),
            ('metallic-reflector', "Metallic Reflector", "", 1),
            ('dielectric', "Dielectric", "", 3),
            ('absorber', "Absorber", "Absorbs all energy.", 4)
        ],
        name="Substance Type",
        description="Type of ideal substance.",
        default='metallic-reflector'
    )

    fresnel_type: bpy.props.EnumProperty(
        items=[
            ('schlick', "Schlick Approximation", "", 0),
            ('exact', "Exact", "", 1)
        ],
        name="Fresnel Type",
        description="Type of Fresnel effect used.",
        default='exact'
    )

    ior_outer: bpy.props.FloatProperty(
        name="IoR Outer",
        default=1.0,
        min=0.0,
        max=sys.float_info.max
    )

    ior_inner: bpy.props.FloatProperty(
        name="IoR Inner",
        default=1.5,
        min=0.0,
        max=sys.float_info.max
    )

    ior_inner_n: bpy.props.FloatVectorProperty(
        name="IoR Inner N",
        description="Real part of the complex IoR.",
        default=[0.0, 0.0, 0.0],
        min=0.0,
        max=sys.float_info.max,
        subtype='COLOR',
        size=3
    )

    ior_inner_k: bpy.props.FloatVectorProperty(
        name="IoR Inner K",
        description="Imaginary part of the complex IoR.",
        default=[0.0, 0.0, 0.0],
        min=0.0,
        max=sys.float_info.max,
        subtype='COLOR',
        size=3
    )

    def to_sdl(self, b_material, sdlconsole):
        creator = sdl.IdealSubstanceMaterialCreator()
        creator.set_data_name(self.get_output_resource_name(b_material))
        creator.set_substance(sdl.Enum(self.substance_type))
        creator.set_fresnel(sdl.Enum(self.fresnel_type))
        creator.set_ior_outer(sdl.Real(self.ior_outer))

        if (
            self.substance_type == 'dielectric-reflector' or
            self.substance_type == 'dielectric-transmitter' or
            self.substance_type == 'dielectric'
        ):
            creator.set_ior_inner(sdl.Real(self.ior_inner))
        
        if self.substance_type == 'metallic-reflector':
            if self.fresnel_type == 'schlick':
                f0_img_name = self.get_linked_input_resource_name(b_material, 0)
                if f0_img_name is not None:
                    creator.set_f0_map(sdl.Image(f0_img_name))
                else:
                    creator.set_f0(sdl.Spectrum(self.get_default_input_value(0)))
            elif self.fresnel_type == "exact":
                creator.set_ior_inner_n(sdl.Spectrum(self.ior_inner_n))
                creator.set_ior_inner_k(sdl.Spectrum(self.ior_inner_k))

        reflection_scale_img_name = self.get_linked_input_resource_name(b_material, 1)
        if reflection_scale_img_name is not None:
            creator.set_reflection_scale_map(sdl.Image(reflection_scale_img_name))
        else:
            creator.set_reflection_scale(sdl.Spectrum(self.get_default_input_value(1)))

        transmission_scale_img_name = self.get_linked_input_resource_name(b_material, 2)
        if transmission_scale_img_name is not None:
            creator.set_transmission_scale_map(sdl.Image(transmission_scale_img_name))
        else:
            creator.set_transmission_scale(sdl.Spectrum(self.get_default_input_value(2)))

        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.inputs.new(PhF0Socket.bl_idname, "F0")
        self.inputs.new(PhReflectionScaleSocket.bl_idname, "Reflection Scale")
        self.inputs.new(PhTransmissionScaleSocket.bl_idname, "Transmission Scale")
        self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, 'substance_type', text="")
        b_layout.prop(self, 'fresnel_type', text="")
        b_layout.prop(self, 'ior_outer')

        f0_socket = self.inputs["F0"]
        reflection_scale_socket = self.inputs["Reflection Scale"]
        transmission_scale_socket = self.inputs["Transmission Scale"]
        f0_socket.hide = True
        reflection_scale_socket.hide = True
        transmission_scale_socket.hide = True

        if self.substance_type == 'dielectric-reflector':
            reflection_scale_socket.hide = False
            b_layout.prop(self, 'ior_inner')
        elif self.substance_type == 'dielectric-transmitter':
            transmission_scale_socket.hide = False
            b_layout.prop(self, 'ior_inner')
        elif self.substance_type == 'metallic-reflector':
            reflection_scale_socket.hide = False
            if self.fresnel_type == "schlick":
                f0_socket.hide = False
            elif self.fresnel_type == "exact":
                b_layout.prop(self, 'ior_inner_n')
                b_layout.prop(self, 'ior_inner_k')
        elif self.substance_type == 'dielectric':
            reflection_scale_socket.hide = False
            transmission_scale_socket.hide = False
            b_layout.prop(self, 'ior_inner')
