from ..node_base import (
    PhSurfaceMaterialNode,
    PhSurfaceMaterialSocket,
    PhFloatFactorSocket,
    PhF0Socket)
from psdl import sdl
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

    is_anisotropic: bpy.props.BoolProperty(
        name="Anisotropic",
        description="",
        default=False
    )

    def to_sdl(self, b_material, sdlconsole):
        creator = sdl.AbradedOpaqueMaterialCreator()
        creator.set_data_name(self.get_output_resource_name(b_material))
        creator.set_microsurface(sdl.Enum("ggx"))

        f0_img_name = self.get_linked_input_resource_name(b_material, 0)
        if f0_img_name is not None:
            creator.set_f0_map(sdl.Image(f0_img_name))
        else:
            creator.set_f0(sdl.Spectrum(self.get_default_input_value(0)))

        if not self.is_anisotropic:
            roughness_img_name = self.get_linked_input_resource_name(b_material, 1)
            if roughness_img_name is not None:
                creator.set_roughness_map(sdl.Image(roughness_img_name))
            else:
                creator.set_roughness(sdl.Real(self.get_default_input_value(1)))
        else:
            roughness_u_img_name = self.get_linked_input_resource_name(b_material, 2)
            if roughness_u_img_name is not None:
                creator.set_roughness_map(sdl.Image(roughness_u_img_name))
            else:
                creator.set_roughness(sdl.Real(self.get_default_input_value(2)))

            roughness_v_img_name = self.get_linked_input_resource_name(b_material, 3)
            if roughness_v_img_name is not None:
                creator.set_roughness_v_map(sdl.Image(roughness_v_img_name))
            else:
                creator.set_roughness_v(sdl.Real(self.get_default_input_value(3)))

        if self.mapping_type == 'SQUARED':
            creator.set_roughness_to_alpha(sdl.Enum("squared"))
        elif self.mapping_type == 'PBRTV3':
            creator.set_roughness_to_alpha(sdl.Enum("pbrt-v3"))
        elif self.mapping_type == 'EQUALED':
            creator.set_roughness_to_alpha(sdl.Enum("equaled"))

        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.inputs.new(PhF0Socket.bl_idname, "F0")
        self.inputs.new(PhFloatFactorSocket.bl_idname, "Roughness")
        self.inputs.new(PhFloatFactorSocket.bl_idname, "Roughness U")
        self.inputs.new(PhFloatFactorSocket.bl_idname, "Roughness V")
        self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, "mapping_type", text="")
        b_layout.prop(self, 'is_anisotropic')
