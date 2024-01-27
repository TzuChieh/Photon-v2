from ..node_base import (
    PhSurfaceMaterialNode,
    PhSurfaceMaterialSocket,
    PhFloatFactorSocket)
from psdl import sdl
from ... import naming

import bpy


class PhAbradedOpaqueNode(PhSurfaceMaterialNode):
    bl_idname = 'PH_ABRADED_OPAQUE'
    bl_label = "Abraded Opaque"

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

        creator = sdl.AbradedOpaqueMaterialCreator()
        creator.set_data_name(surface_mat_res_name)
        creator.set_microsurface(sdl.Enum("ggx"))
        creator.set_f0(sdl.Spectrum(self.f0))

        if not self.is_anisotropic:
            creator.set_roughness(sdl.Real(self.inputs[0].default_value))
        else:
            creator.set_roughness(sdl.Real(self.inputs[1].default_value))
            creator.set_roughness_v(sdl.Real(self.inputs[2].default_value))

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

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, "mapping_type", text="")
        b_layout.prop(self, 'f0')
        b_layout.prop(self, 'is_anisotropic')
