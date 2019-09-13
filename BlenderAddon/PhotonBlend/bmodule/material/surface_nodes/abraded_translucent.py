from ..node_base import (
        PhMaterialNode,
        PhSurfaceMaterialSocket,
        SURFACE_MATERIAL_CATEGORY)
from ....psdl.pysdl import (
        AbradedTranslucentMaterialCreator,
        SDLString,
        SDLReal)
from ... import naming

import bpy

import sys


class PhAbradedTranslucentNode(PhMaterialNode):
    bl_idname = 'PH_ABRADED_TRANSLUCENT'
    bl_label = "Abraded Translucent"
    node_category = SURFACE_MATERIAL_CATEGORY

    fresnel_type: bpy.props.EnumProperty(
        items=[
            ('SCHLICK', "Schlick Approximation", ""),
            ('EXACT', "Exact", "")
        ],
        name="Fresnel Type",
        description="Type of Fresnel effect used.",
        default='EXACT'
    )

    roughness: bpy.props.FloatProperty(
        name="Roughness",
        default=0.5,
        min=0.0,
        max=1.0
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

    def to_sdl(self, b_material, sdlconsole):
        surface_mat_socket = self.outputs[0]
        surface_mat_res_name = naming.get_mangled_output_node_socket_name(surface_mat_socket, b_material)

        creator = AbradedTranslucentMaterialCreator()
        creator.set_data_name(surface_mat_res_name)
        creator.set_roughness(SDLReal(self.roughness))
        creator.set_ior_inner(SDLReal(self.ior_inner))
        creator.set_ior_outer(SDLReal(self.ior_outer))
        if self.fresnel_type == 'SCHLICK':
            creator.set_fresnel_model(SDLString("schlick"))
        elif self.fresnel_type == 'EXACT':
            creator.set_fresnel_model(SDLString("exact"))
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)

        self.width *= 1.2

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, 'fresnel_type', text="")
        b_layout.prop(self, 'roughness')
        b_layout.prop(self, 'ior_outer')
        b_layout.prop(self, 'ior_inner')
