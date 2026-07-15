from ..node_base import (
        PhMaterialConversionNode,
        PhColorSocket,
        PhFloatValueSocket)
from psdl import sdl


class PhSplitImageNode(PhMaterialConversionNode):
    bl_idname = 'PH_SPLIT_IMAGE'
    bl_label = "Split Image"

    output_subscripts = ('r', 'g', 'b', 'a')

    def to_sdl(self, b_material, sdlconsole):
        linked_output_indices = [
            output_index
            for output_index, output_socket in enumerate(self.outputs)
            if output_socket.is_linked]
        if not linked_output_indices:
            return

        input_res_name = self.get_linked_input_resource_name(b_material, 0)
        if not input_res_name:
            input_res_name = self.get_default_input_resource_name(b_material, 0)
            input_creator = sdl.ConstantImageCreator()
            input_creator.set_data_name(input_res_name)
            input_creator.set_values(sdl.RealArray(self.get_default_input_value(0)))
            input_creator.set_color_space(sdl.Enum('LSRGB'))
            sdlconsole.queue_command(input_creator)

        for output_index in linked_output_indices:
            creator = sdl.SwizzledImageImageCreator()
            creator.set_data_name(self.get_output_resource_name(b_material, output_index))
            creator.set_input(sdl.Image(input_res_name))
            creator.set_swizzle_subscripts(sdl.String(self.output_subscripts[output_index]))
            sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.inputs.new(PhColorSocket.bl_idname, PhColorSocket.bl_label)

        for output_name in ('R', 'G', 'B', 'A'):
            self.outputs.new(PhFloatValueSocket.bl_idname, output_name)
