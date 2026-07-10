from ..node_base import (
        PhSurfaceMaterialNode,
        PhSurfaceMaterialSocket,
        PhFloatFactorSocket)
from psdl import sdl

import bpy

import sys


class PhAbradedTranslucentNode(PhSurfaceMaterialNode):
    bl_idname = 'PH_ABRADED_TRANSLUCENT'
    bl_label = "Abraded Translucent"

    mapping_type: bpy.props.EnumProperty(
        items=[
            ('SQUARED', "Squared", "", 0),
            ('PBRTV3', "pbrt-v3", "", 1),
            ('EQUALED', "Equaled", "", 2)
        ],
        name="Roughness Mapping Type",
        description="",
        default='SQUARED'
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

    is_anisotropic: bpy.props.BoolProperty(
        name="Anisotropic",
        description="",
        default=False
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
        creator = sdl.AbradedTranslucentMaterialCreator()
        creator.set_data_name(self.get_output_resource_name(b_material))
        creator.set_ior_inner(sdl.Real(self.ior_inner))
        creator.set_ior_outer(sdl.Real(self.ior_outer))
        creator.set_fresnel(sdl.Enum(self.fresnel_type))

        if not self.is_anisotropic:
            roughness_img_name = self.get_linked_input_resource_name(b_material, 0)
            if roughness_img_name is not None:
                creator.set_roughness_map(sdl.Image(roughness_img_name))
            else:
                creator.set_roughness(sdl.Real(self.get_default_input_value(0)))
        else:
            roughness_u_img_name = self.get_linked_input_resource_name(b_material, 1)
            if roughness_u_img_name is not None:
                creator.set_roughness_map(sdl.Image(roughness_u_img_name))
            else:
                creator.set_roughness(sdl.Real(self.get_default_input_value(1)))

            roughness_v_img_name = self.get_linked_input_resource_name(b_material, 2)
            if roughness_v_img_name is not None:
                creator.set_roughness_v_map(sdl.Image(roughness_v_img_name))
            else:
                creator.set_roughness_v(sdl.Real(self.get_default_input_value(2)))

        if self.mapping_type == 'SQUARED':
            creator.set_roughness_to_alpha(sdl.Enum("squared"))
        elif self.mapping_type == 'PBRTV3':
            creator.set_roughness_to_alpha(sdl.Enum("pbrt-v3"))
        elif self.mapping_type == 'EQUALED':
            creator.set_roughness_to_alpha(sdl.Enum("equaled"))

        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.inputs.new(PhFloatFactorSocket.bl_idname, "Roughness")
        self.inputs.new(PhFloatFactorSocket.bl_idname, "Roughness U")
        self.inputs.new(PhFloatFactorSocket.bl_idname, "Roughness V")
        self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)

        self.width *= 1.2

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, "mapping_type", text="")
        b_layout.prop(self, 'is_anisotropic')
        b_layout.prop(self, 'fresnel_type', text="")
        b_layout.prop(self, 'ior_outer')
        b_layout.prop(self, 'ior_inner')
