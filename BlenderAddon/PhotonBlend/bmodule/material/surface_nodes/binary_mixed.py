from ..node_base import (
        PhMaterialNode,
        PhSurfaceMaterialSocket,
        SURFACE_MATERIAL_CATEGORY)
from ....psdl.pysdl import (
        BinaryMixedSurfaceMaterialCreator,
        SDLMaterial,
        SDLReal)
from ... import naming

import bpy


class PhBinaryMixedSurfaceNode(PhMaterialNode):
    bl_idname = "PH_BINARY_MIXED_SURFACE"
    bl_label = "Binary Mixed Surface"
    node_category = SURFACE_MATERIAL_CATEGORY

    factor: bpy.props.FloatProperty(
        name="Factor",
        default=0.5,
        min=0.0,
        max=1.0
    )

    def to_sdl(self, b_material, sdlconsole):
        mat0_socket = self.inputs[0]
        mat1_socket = self.inputs[1]
        surface_mat_socket = self.outputs[0]

        mat0_res_name = mat0_socket.get_from_res_name(b_material)
        mat1_res_name = mat1_socket.get_from_res_name(b_material)
        if mat0_res_name is None or mat1_res_name is None:
            print("warning: material <%s>'s binary mixed surface node is incomplete" % b_material.name)
            return

        creator = BinaryMixedSurfaceMaterialCreator()
        creator.set_data_name(naming.get_mangled_output_node_socket_name(surface_mat_socket, b_material))
        creator.set_factor(SDLReal(self.factor))
        creator.set_material_0(SDLMaterial(mat0_res_name))
        creator.set_material_1(SDLMaterial(mat1_res_name))
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.inputs.new(PhSurfaceMaterialSocket.bl_idname, "Material A")
        self.inputs.new(PhSurfaceMaterialSocket.bl_idname, "Material B")
        self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)

    def draw_buttons(self, b_context, b_layout):
        row = b_layout.row()
        row.prop(self, "factor")
