from ..node_base import (
        PhMaterialNode,
        PhSurfaceMaterialSocket,
        PhColorSocket,
        OUTPUT_CATEGORY)
from ....psdl.pysdl import (
        FullMaterialCreator,
        SDLMaterial)
from ... import naming


class PhOutputNode(PhMaterialNode):
    bl_idname = "PH_OUTPUT"
    bl_label = "Output"
    node_category = OUTPUT_CATEGORY

    def to_sdl(self, b_material, sdlconsole):
        surface_mat_socket = self.inputs[0]
        surface_mat_res_name = surface_mat_socket.get_from_res_name(b_material)
        if surface_mat_res_name is None:
            print("material <%s>'s output node is not linked, ignored" % b_material.name)
            return

        creator = FullMaterialCreator()
        creator.set_data_name(naming.get_mangled_material_name(b_material))
        creator.set_surface(SDLMaterial(surface_mat_res_name))
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.inputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)
        self.inputs.new(PhColorSocket.bl_idname, "Surface Emission")
        self.inputs[1].link_only = True

    def get_surface_emi_res_name(self, b_material):
        surface_emi_socket = self.inputs[1]
        return surface_emi_socket.get_from_res_name(b_material)
