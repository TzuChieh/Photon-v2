from ..node_base import (
    PhMaterialNode,
    PhSurfaceMaterialSocket,
    PhFloatFactorSocket,
    SURFACE_MATERIAL_CATEGORY)
from ....psdl.pysdl import (
        AbradedOpaqueMaterialCreator,
        SDLString,
        SDLVector3,
        SDLReal)
from ... import naming

import bpy
import mathutils


class PhAbradedOpaqueNode(PhMaterialNode):
    bl_idname = "PH_ABRADED_OPAQUE"
    bl_label = "Abraded Opaque"
    node_category = SURFACE_MATERIAL_CATEGORY

    mapping_type: bpy.props.EnumProperty(
        items=[
            ('SQUARED', "Squared", ""),
            ('PBRTV3', "pbrt-v3", ""),
            ('EQUALED', "Equaled", "")
        ],
        name="Roughness Mapping Type",
        description="",
        default='SQUARED'
    )

    f0: bpy.props.FloatVectorProperty(
        name="Color",
        description="color value",
        default=[0.5, 0.5, 0.5],
        min=0.0,
        max=1.0,
        subtype="COLOR",
        size=3
    )

    is_anisotropic: bpy.props.BoolProperty(
        name="Anisotropic",
        description="",
        default=False
    )

    def to_sdl(self, b_material, sdlconsole):
        surface_mat_socket = self.outputs[0]
        surface_mat_res_name = naming.get_mangled_output_node_socket_name(surface_mat_socket, b_material)

        creator = AbradedOpaqueMaterialCreator()
        creator.set_data_name(surface_mat_res_name)
        creator.set_distribution_model(SDLString("ggx"))
        creator.set_f0(SDLVector3(mathutils.Color((self.f0[0], self.f0[1], self.f0[2]))))

        if not self.is_anisotropic:
            creator.set_roughness(SDLReal(self.inputs[0].default_value))
        else:
            creator.set_roughness_u(SDLReal(self.inputs[1].default_value))
            creator.set_roughness_v(SDLReal(self.inputs[2].default_value))

        if self.mapping_type == 'SQUARED':
            creator.set_mapping(SDLString("squared"))
        elif self.mapping_type == 'PBRTV3':
            creator.set_mapping(SDLString("pbrt-v3"))
        elif self.mapping_type == 'EQUALED':
            creator.set_mapping(SDLString("equaled"))

        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.inputs.new(PhFloatFactorSocket.bl_idname, "Roughness")
        self.inputs.new(PhFloatFactorSocket.bl_idname, "Roughness U")
        self.inputs.new(PhFloatFactorSocket.bl_idname, "Roughness V")
        self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, "mapping_type", text="")
        b_layout.prop(self, 'f0')
        b_layout.prop(self, 'is_anisotropic')
