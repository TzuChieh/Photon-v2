from ..node_base import (
        PhMaterialNode,
        PhSurfaceMaterialSocket,
        PhSurfaceLayerSocket,
        SURFACE_MATERIAL_CATEGORY)
from ....generated.pysdl import (
        LayeredSurfaceMaterialCreator,
        LayeredSurfaceMaterialSet,
        LayeredSurfaceMaterialAdd,
        SDLVector3,
        SDLReal,
        SDLInteger)
from ... import naming

import bpy


class PhLayeredSurfaceNode(PhMaterialNode):
    bl_idname = "PH_LAYERED_SURFACE"
    bl_label = "Layered Surface"
    node_category = SURFACE_MATERIAL_CATEGORY

    def update_inputs(self, b_context):
        specified_num_layers = self["num_layers"]

        while len(self.inputs) != specified_num_layers:
            if len(self.inputs) < specified_num_layers:
                self.inputs.new(PhSurfaceLayerSocket.bl_idname, PhSurfaceLayerSocket.bl_label)
            else:
                self.inputs.remove(self.inputs[len(self.inputs) - 1])

    num_layers: bpy.props.IntProperty(
        name="# Layers",
        default=1,
        min=1,
        max=1024,
        update=update_inputs
    )

    def to_sdl(self, b_material, sdlconsole):
        surface_mat_socket = self.outputs[0]
        surface_mat_res_name = naming.get_mangled_output_node_socket_name(surface_mat_socket, b_material)

        creator = LayeredSurfaceMaterialCreator()
        creator.set_data_name(surface_mat_res_name)
        sdlconsole.queue_command(creator)

        for i in range(0, len(self.inputs)):
            if not self.inputs[i].links:
                continue

            layer_node = self.inputs[i].links[0].from_node

            adder = LayeredSurfaceMaterialAdd()
            adder.set_target_name(surface_mat_res_name)
            sdlconsole.queue_command(adder)

            setter = LayeredSurfaceMaterialSet()
            setter.set_target_name(surface_mat_res_name)
            setter.set_index(SDLInteger(i))

            setter.set_roughness(SDLReal(layer_node.roughness))
            if layer_node.ior_type == "SCALAR":
                setter.set_ior_n(SDLReal(layer_node.ior_n))
                setter.set_ior_k(SDLReal(layer_node.ior_k))
            elif layer_node.ior_type == "RGB":
                setter.set_ior_n(SDLVector3(layer_node.ior_n_rgb))
                setter.set_ior_k(SDLVector3(layer_node.ior_k_rgb))
            setter.set_depth(SDLReal(layer_node.depth))
            setter.set_g(SDLReal(layer_node.g))
            setter.set_sigma_a(SDLReal(layer_node.sigma_a))
            setter.set_sigma_s(SDLReal(layer_node.sigma_s))

            sdlconsole.queue_command(setter)

    def init(self, b_context):
        self.inputs.new(PhSurfaceLayerSocket.bl_idname, PhSurfaceLayerSocket.bl_label)
        self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, "num_layers")
