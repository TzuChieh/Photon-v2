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

    f0: bpy.props.FloatVectorProperty(
        name="Color",
        description="color value",
        default=[0.5, 0.5, 0.5],
        min=0.0,
        max=1.0,
        subtype="COLOR",
        size=3
    )

    def to_sdl(self, b_material, sdlconsole):
        surface_mat_socket = self.outputs[0]
        surface_mat_res_name = naming.get_mangled_output_node_socket_name(surface_mat_socket, b_material)

        creator = AbradedOpaqueMaterialCreator()
        creator.set_data_name(surface_mat_res_name)
        creator.set_type(SDLString("iso-metallic-ggx"))
        creator.set_roughness(SDLReal(self.inputs[0].default_value))
        creator.set_f0(SDLVector3(mathutils.Color((self.f0[0], self.f0[1], self.f0[2]))))
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.inputs.new(PhFloatFactorSocket.bl_idname, "Roughness")
        self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, "f0")
