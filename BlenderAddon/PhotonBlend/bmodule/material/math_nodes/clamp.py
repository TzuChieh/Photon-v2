from ..node_base import (
        PhMaterialMathNode,
        PhColorSocket,
        PhColorSocketWithFloatDefault)
from psdl import sdl

import bpy


class PhClampNode(PhMaterialMathNode):
    bl_idname = 'PH_CLAMP'
    bl_label = "Clamp"

    def to_sdl(self, b_material, sdlconsole):
        value_color_res_name = self.get_linked_input_resource_name(b_material, 0)
        if not value_color_res_name:
            value_color_res_name = self.get_default_input_resource_name(b_material, 0)
            creator = sdl.ConstantImageCreator()
            creator.set_data_name(value_color_res_name)
            creator.set_values(sdl.RealArray([self.get_default_input_value(0)]))
            sdlconsole.queue_command(creator)

        lower_bound_color_res_name = self.get_linked_input_resource_name(b_material, 1)
        upper_bound_color_res_name = self.get_linked_input_resource_name(b_material, 2)

        creator = sdl.MathImageCreator()
        creator.set_data_name(self.get_output_resource_name(b_material))
        creator.set_math_image_op(sdl.Enum("clamp"))
        creator.set_operand(sdl.Image(value_color_res_name))

        if lower_bound_color_res_name:
            creator.set_input_0(sdl.Image(lower_bound_color_res_name))
        else:
            creator.set_scalar_input_0(sdl.Real(self.get_default_input_value(1)))

        if upper_bound_color_res_name:
            creator.set_input_1(sdl.Image(upper_bound_color_res_name))
        else:
            creator.set_scalar_input_1(sdl.Real(self.get_default_input_value(2)))

        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.inputs.new(PhColorSocketWithFloatDefault.bl_idname, "Value")
        lower_bound_socket = self.inputs.new(PhColorSocketWithFloatDefault.bl_idname, "Lower Bound")
        upper_bound_socket = self.inputs.new(PhColorSocketWithFloatDefault.bl_idname, "Upper Bound")
        self.outputs.new(PhColorSocket.bl_idname, PhColorSocket.bl_label)

        lower_bound_socket.default_value = 0
        upper_bound_socket.default_value = 1
