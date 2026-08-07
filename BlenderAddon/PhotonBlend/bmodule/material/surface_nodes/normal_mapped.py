import bpy

from ..node_base import (
    PhSurfaceMaterialNode,
    PhSurfaceMaterialSocket,
    PhColorSocket,
    PhFloatValueSocket)
from psdl import sdl


class PhNormalMappedSurfaceNode(PhSurfaceMaterialNode):
    bl_idname = 'PH_NORMAL_MAPPED_SURFACE'
    bl_label = "Normal Mapped Surface"

    normal_map_format: bpy.props.EnumProperty(
        name="Format",
        description="Normal map channel convention",
        items=[
            ('opengl', "OpenGL", "RGB stores +x, +y, +z", 0),
            ('directx', "DirectX", "RGB stores +x, -y, +z", 1),
            ('directx-rg', "DirectX RG", "RG stores +x, -y; +z is reconstructed", 2)],
        default='opengl')

    def to_sdl(self, b_material, sdlconsole):
        material_res_name = self.get_linked_input_resource_name(b_material, 0)
        normal_map_res_name = self.get_linked_input_resource_name(b_material, 1)
        strength_map_res_name = self.get_linked_input_resource_name(b_material, 2)
        if material_res_name is None or normal_map_res_name is None:
            self.warn_incomplete_node(b_material, "surface material or normal map input is not linked")
            self.queue_fallback_output_material(b_material, sdlconsole)
            return

        creator = sdl.SurfaceNormalMapMaterialCreator()
        creator.set_data_name(self.get_output_resource_name(b_material))
        creator.set_material(sdl.Material(material_res_name))
        creator.set_map(sdl.Image(normal_map_res_name))
        creator.set_format(sdl.Enum(self.normal_map_format))
        if strength_map_res_name:
            creator.set_strength_map(sdl.Image(strength_map_res_name))
        else:
            creator.set_strength(sdl.Real(self.get_default_input_value(2)))
        sdlconsole.queue_command(creator)

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, 'normal_map_format', text="")

    def init(self, b_context):
        self.inputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)
        self.inputs[0].link_only = True

        self.inputs.new(PhColorSocket.bl_idname, "Normal Map")
        self.inputs[1].link_only = True

        strength_socket = self.inputs.new(PhFloatValueSocket.bl_idname, "Strength")
        strength_socket.default_value = 1.0
        self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)
