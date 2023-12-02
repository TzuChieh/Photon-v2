from ..node_base import (
        PhSurfaceMaterialNode,
        PhSurfaceMaterialSocket)
from psdl import sdl
from ... import naming

import bpy

import sys


class PhIdealSubstanceNode(PhSurfaceMaterialNode):
    bl_idname = 'PH_IDEAL_SUBSTANCE'
    bl_label = "Ideal Substance"

    substance_type: bpy.props.EnumProperty(
        items=[
            ("DIELECTRIC_REFLECTOR", "Dielectric Reflector", ""),
            ("METALLIC_REFLECTOR", "Metallic Reflector", ""),
            ("DIELECTRIC_TRANSMITTER", "Dielectric Transmitter", ""),
            ("DIELECTRIC", "Dielectric", "")
        ],
        name="Substance Type",
        description="Type of ideal substance.",
        default="METALLIC_REFLECTOR"
    )

    f0: bpy.props.FloatVectorProperty(
        name="F0",
        description="F0 value",
        default=[0.9, 0.9, 0.9],
        min=0.0,
        max=1.0,
        subtype="COLOR",
        size=3
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

    reflection_scale: bpy.props.FloatVectorProperty(
        name="Reflection Scale",
        description="for artistic control",
        default=[1.0, 1.0, 1.0],
        min=0.0,
        max=sys.float_info.max,
        subtype="COLOR",
        size=3
    )

    transmission_scale: bpy.props.FloatVectorProperty(
        name="Transmission Scale",
        description="for artistic control",
        default=[1.0, 1.0, 1.0],
        min=0.0,
        max=1.0,
        subtype="COLOR",
        size=3
    )

    def to_sdl(self, b_material, sdlconsole):

        surface_mat_socket = self.outputs[0]
        surface_mat_res_name = naming.get_mangled_output_node_socket_name(surface_mat_socket, b_material)

        creator = sdl.IdealSubstanceMaterialCreator()
        creator.set_data_name(surface_mat_res_name)
        creator.set_ior_outer(sdl.Real(self.ior_outer))
        creator.set_ior_inner(sdl.Real(self.ior_inner))
        creator.set_f0(sdl.Spectrum(self.f0))
        creator.set_reflection_scale(sdl.Vector3(self.reflection_scale))
        creator.set_transmission_scale(sdl.Vector3(self.transmission_scale))

        if self.substance_type == "DIELECTRIC_REFLECTOR":
            creator.set_substance(sdl.Enum("dielectric-reflector"))
        elif self.substance_type == "METALLIC_REFLECTOR":
            creator.set_substance(sdl.Enum("metallic-reflector"))
        elif self.substance_type == "DIELECTRIC_TRANSMITTER":
            creator.set_substance(sdl.Enum("transmitter"))
        elif self.substance_type == "DIELECTRIC":
            creator.set_substance(sdl.Enum("dielectric"))

        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, "substance_type", text="")
        b_layout.prop(self, "ior_outer")

        if (
            self.substance_type == "DIELECTRIC_REFLECTOR" or
            self.substance_type == "DIELECTRIC_TRANSMITTER" or
            self.substance_type == "DIELECTRIC"
        ):
            b_layout.prop(self, "ior_inner")

        if self.substance_type == "METALLIC_REFLECTOR":
            b_layout.prop(self, "f0")

        b_layout.prop(self, "reflection_scale")
        b_layout.prop(self, "transmission_scale")
