from ..node_base import (
        PhSurfaceMaterialNode,
        PhSurfaceMaterialSocket,
        PhSurfaceLayerSocket)
from psdl import sdl
from ... import naming

import bpy


class PhLayeredSurfaceNode(PhSurfaceMaterialNode):
    bl_idname = 'PH_LAYERED_SURFACE'
    bl_label = "Layered Surface"

    def update_inputs(self, b_context):
        specified_num_layers = self['num_layers']

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
        # Generate one packet for each layer
        packets = []
        for i in range(0, len(self.inputs)):
            if not self.inputs[i].links:
                continue

            layer_node = self.inputs[i].links[0].from_node
            packet_name = "layer_%d_data" % i

            sdlconsole.queue_command(layer_node.make_cached_packet_command(packet_name))
            packets.append(sdl.CachedPacket(packet_name))

        # Generate layered surface material

        surface_mat_socket = self.outputs[0]
        surface_mat_res_name = naming.get_mangled_output_node_socket_name(surface_mat_socket, b_material)

        creator = sdl.LayeredSurfaceMaterialCreator()
        creator.set_data_name(surface_mat_res_name)
        creator.set_layers(sdl.StructArray(packets))
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.inputs.new(PhSurfaceLayerSocket.bl_idname, PhSurfaceLayerSocket.bl_label)
        self.outputs.new(PhSurfaceMaterialSocket.bl_idname, PhSurfaceMaterialSocket.bl_label)

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, 'num_layers')
