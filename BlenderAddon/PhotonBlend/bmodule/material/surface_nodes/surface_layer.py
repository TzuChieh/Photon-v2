from ..node_base import (
        PhSurfaceMaterialNode,
        PhSurfaceLayerSocket)
from psdl import sdl

import bpy

import sys


class PhSurfaceLayerNode(PhSurfaceMaterialNode):
    bl_idname = 'PH_SURFACE_LAYER'
    bl_label = "Surface Layer"

    roughness: bpy.props.FloatProperty(
        name="Roughness",
        default=0.5,
        min=0.0,
        max=1.0
    )

    ior_type: bpy.props.EnumProperty(
        items=[
            ('SCALAR', "Scalar", "Scalar.", 0),
            ('VECTOR', "Vector", "Vector values for tristimulus render mode.", 1)
        ],
        name="IoR Type",
        description="Type of IoR data used.",
        default='SCALAR'
    )

    ior_n_scalar: bpy.props.FloatProperty(
        name="IoR N",
        default=1.5,
        min=0.0,
        max=sys.float_info.max
    )

    ior_k_scalar: bpy.props.FloatProperty(
        name="IoR K",
        default=0.0,
        min=0.0,
        max=sys.float_info.max
    )

    ior_n_vector: bpy.props.FloatVectorProperty(
        name="IoR N",
        description="Vector values of IoR N.",
        default=[1.5, 1.5, 1.5],
        min=0.0,
        max=sys.float_info.max,
        subtype='COLOR',
        size=3
    )

    ior_k_vector: bpy.props.FloatVectorProperty(
        name="IoR K",
        description="Vector values of IoR K.",
        default=[0.0, 0.0, 0.0],
        min=0.0,
        max=sys.float_info.max,
        subtype='COLOR',
        size=3
    )

    depth: bpy.props.FloatProperty(
        name="depth",
        default=0.0,
        min=0.0,
        max=sys.float_info.max
    )

    g: bpy.props.FloatProperty(
        name="g",
        default=0.9,
        min=0.5,
        max=1.0
    )

    sigma_a: bpy.props.FloatProperty(
        name="Sigma A",
        default=0.1,
        min=0.0,
        max=sys.float_info.max
    )

    sigma_s: bpy.props.FloatProperty(
        name="Sigma S",
        default=0.1,
        min=0.0,
        max=sys.float_info.max
    )

    def to_sdl(self, b_material, sdlconsole):
        pass

    def init(self, b_context):
        self.outputs.new(PhSurfaceLayerSocket.bl_idname, PhSurfaceLayerSocket.bl_label)

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, 'ior_type')
        b_layout.prop(self, 'roughness')

        if self.ior_type == 'SCALAR':
            b_layout.prop(self, 'ior_n_scalar')
            b_layout.prop(self, 'ior_k_scalar')
        elif self.ior_type == 'VECTOR':
            b_layout.prop(self, 'ior_n_vector')
            b_layout.prop(self, 'ior_k_vector')

        b_layout.prop(self, 'depth')
        b_layout.prop(self, 'g')
        b_layout.prop(self, 'sigma_a')
        b_layout.prop(self, 'sigma_s')

    def make_cached_packet_command(self, packet_name):
        packet = sdl.CachedPacketCommand()
        packet.set_data_name(packet_name)
        packet.set_input('roughness', sdl.Real(self.roughness))

        if self.ior_type == 'SCALAR':
            packet.set_input('ior-n', sdl.Spectrum([self.ior_n_scalar]))
            packet.set_input('ior-k', sdl.Spectrum([self.ior_k_scalar]))
        elif self.ior_type == 'VECTOR':
            packet.set_input('ior-n', sdl.Spectrum(self.ior_n_vector))
            packet.set_input('ior-k', sdl.Spectrum(self.ior_k_vector))

        packet.set_input('depth', sdl.Real(self.depth))
        packet.set_input('g', sdl.Real(self.g))
        packet.set_input('sigma-a', sdl.Real(self.sigma_a))
        packet.set_input('sigma-s', sdl.Real(self.sigma_s))

        return packet
