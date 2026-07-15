from ..node_base import (
        PhMaterialConversionNode,
        PhColorSocket,
        PhFloatValueSocket)
from psdl import sdl


class PhLuminanceNode(PhMaterialConversionNode):
    bl_idname = 'PH_LUMINANCE'
    bl_label = "Luminance"

    def to_sdl(self, b_material, sdlconsole):
        if not self.outputs[0].is_linked:
            return

        input_res_name = self.get_linked_input_resource_name(b_material, 0)
        if not input_res_name:
            input_res_name = self.get_default_input_resource_name(b_material, 0)

            input_creator = sdl.ConstantImageCreator()
            input_creator.set_data_name(input_res_name)
            input_creator.set_values(sdl.RealArray(self.get_default_input_value(0)))
            input_creator.set_color_space(sdl.Enum('LSRGB'))
            sdlconsole.queue_command(input_creator)

        creator = sdl.LuminanceImageCreator()
        creator.set_data_name(self.get_output_resource_name(b_material))
        creator.set_input(sdl.Image(input_res_name))
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.inputs.new(PhColorSocket.bl_idname, PhColorSocket.bl_label)
        self.outputs.new(PhFloatValueSocket.bl_idname, PhFloatValueSocket.bl_label)
