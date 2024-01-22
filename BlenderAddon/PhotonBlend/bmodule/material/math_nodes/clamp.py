from ..node_base import (
        PhMaterialMathNode,
        PhColorSocket,
        PhColorSocketWithFloatDefault)
from psdl import sdl
from ... import naming

import bpy


class PhClampNode(PhMaterialMathNode):
    bl_idname = 'PH_CLAMP'
    bl_label = "Clamp"

    def to_sdl(self, b_material, sdlconsole):
        value_socket       = self.inputs[0]
        lower_bound_socket = self.inputs[1]
        upper_bound_socket = self.inputs[2]
        output_socket      = self.outputs[0]

        value_color_res_name = value_socket.get_from_res_name(b_material)
        if not value_color_res_name:
            value_color_res_name = naming.get_mangled_input_node_socket_name(value_socket, b_material)
            creator = sdl.ConstantImageCreator()
            creator.set_data_name(value_color_res_name)
            creator.set_values(sdl.RealArray([value_socket.default_value]))
            sdlconsole.queue_command(creator)

        lower_bound_color_res_name = lower_bound_socket.get_from_res_name(b_material)
        upper_bound_color_res_name = upper_bound_socket.get_from_res_name(b_material)
        output_color_res_name = naming.get_mangled_output_node_socket_name(output_socket, b_material)

        creator = sdl.MathImageCreator()
        creator.set_data_name(output_color_res_name)
        creator.set_math_image_op(sdl.Enum("clamp"))
        creator.set_operand(sdl.Image(value_color_res_name))

        if lower_bound_color_res_name:
            creator.set_input_0(sdl.Image(lower_bound_color_res_name))
        else:
            creator.set_scalar_input_0(sdl.Real(lower_bound_socket.default_value))

        if upper_bound_color_res_name:
            creator.set_input_1(sdl.Image(upper_bound_color_res_name))
        else:
            creator.set_scalar_input_1(sdl.Real(upper_bound_socket.default_value))

        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.inputs.new(PhColorSocketWithFloatDefault.bl_idname, "Value")
        lower_bound_socket = self.inputs.new(PhColorSocketWithFloatDefault.bl_idname, "Lower Bound")
        upper_bound_socket = self.inputs.new(PhColorSocketWithFloatDefault.bl_idname, "Upper Bound")
        self.outputs.new(PhColorSocket.bl_idname, PhColorSocket.bl_label)

        lower_bound_socket.default_value = 0
        upper_bound_socket.default_value = 1
