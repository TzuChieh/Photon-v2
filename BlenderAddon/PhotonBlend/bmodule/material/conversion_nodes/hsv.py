from ..node_base import (
    PhMaterialConversionNode,
    PhColorSocket,
    PhFloatFactorSocket,
    PhFloatValueSocket)
from psdl import sdl


class PhHsvNode(PhMaterialConversionNode):
    bl_idname = 'PH_HSV'
    bl_label = "HSV"

    def _set_image_or_value(self, b_material, input_index, map_setter, value_setter):
        image_name = self.get_linked_input_resource_name(b_material, input_index)
        if image_name is not None:
            map_setter(sdl.Image(image_name))
        else:
            value_setter(sdl.Real(self.get_default_input_value(input_index)))

    def to_sdl(self, b_material, sdlconsole):
        if not self.outputs[0].is_linked:
            return

        color_input_index = 0
        input_res_name = self.get_linked_input_resource_name(b_material, color_input_index)
        if input_res_name is None:
            input_res_name = self.get_default_input_resource_name(b_material, color_input_index)

            input_creator = sdl.ConstantImageCreator()
            input_creator.set_data_name(input_res_name)
            input_creator.set_values(sdl.RealArray(self.get_default_input_value(color_input_index)))
            input_creator.set_color_space(sdl.Enum('LSRGB'))
            sdlconsole.queue_command(input_creator)

        creator = sdl.HsvImageCreator()
        creator.set_data_name(self.get_output_resource_name(b_material))
        creator.set_input(sdl.Image(input_res_name))
        self._set_image_or_value(b_material, 1, creator.set_hue_map, creator.set_hue)
        self._set_image_or_value(b_material, 2, creator.set_saturation_map, creator.set_saturation)
        self._set_image_or_value(b_material, 3, creator.set_value_map, creator.set_value)
        self._set_image_or_value(b_material, 4, creator.set_amount_map, creator.set_amount)
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        color_socket = self.inputs.new(PhColorSocket.bl_idname, PhColorSocket.bl_label)

        hue_socket = self.inputs.new(PhFloatValueSocket.bl_idname, "Hue")
        hue_socket.default_value = 0.0

        saturation_socket = self.inputs.new(PhFloatValueSocket.bl_idname, "Saturation")
        saturation_socket.default_value = 1.0

        value_socket = self.inputs.new(PhFloatValueSocket.bl_idname, "Value")
        value_socket.default_value = 1.0

        amount_socket = self.inputs.new(PhFloatFactorSocket.bl_idname, "Amount")
        amount_socket.default_value = 1.0

        self.outputs.new(PhColorSocket.bl_idname, PhColorSocket.bl_label)
