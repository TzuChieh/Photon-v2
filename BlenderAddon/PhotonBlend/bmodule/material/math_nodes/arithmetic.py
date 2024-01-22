from ..node_base import (
        PhMaterialMathNode,
        PhColorSocket,
        PhColorSocketWithFloatDefault)
from psdl import sdl
from ... import naming

import bpy


class PhArithmeticNode(PhMaterialMathNode):
    bl_idname = 'PH_ARITHMETIC'
    bl_label = "Arithmetic"

    operation_type_items = [
        ('add', "Add", "A + B", 0),
        ('sub', "Subtract", " A - B", 2),
        ('mul', "Multiply", "A * B", 1),
        ('div', "Division", "A / B", 3),
        ('pow', "Power", "A ^ B", 4),
        ('abs', "Absolute", "|A|", 5),
    ]

    operation_type: bpy.props.EnumProperty(
        items=operation_type_items,
        name="Operation",
        description="Type of the arithmetic operation.",
        default='mul'
    )

    def to_sdl(self, b_material, sdlconsole):
        operand_socket = self.inputs[0]
        input0_socket  = self.inputs[1]
        output_socket  = self.outputs[0]

        operand_color_res_name = operand_socket.get_from_res_name(b_material)
        if not operand_color_res_name:
            operand_color_res_name = naming.get_mangled_input_node_socket_name(operand_socket, b_material)
            creator = sdl.ConstantImageCreator()
            creator.set_data_name(operand_color_res_name)
            creator.set_values(sdl.RealArray([operand_socket.default_value]))
            sdlconsole.queue_command(creator)

        input0_color_res_name = input0_socket.get_from_res_name(b_material)
        output_color_res_name = naming.get_mangled_output_node_socket_name(output_socket, b_material)

        creator = sdl.MathImageCreator()
        creator.set_data_name(output_color_res_name)
        creator.set_math_image_op(sdl.Enum(self.operation_type))
        creator.set_operand(sdl.Image(operand_color_res_name))

        if input0_color_res_name:
            creator.set_input_0(sdl.Image(input0_color_res_name))
        else:
            creator.set_scalar_input_0(sdl.Real(input0_socket.default_value))

        sdlconsole.queue_command(creator)

    def init(self, b_context):
        socket_a = self.inputs.new(PhColorSocketWithFloatDefault.bl_idname, "Value A")
        socket_b = self.inputs.new(PhColorSocketWithFloatDefault.bl_idname, "Value B")
        self.outputs.new(PhColorSocket.bl_idname, PhColorSocket.bl_label)

        # Default op is mul, this make it effectively a no-op on create
        socket_a.default_value = 1
        socket_b.default_value = 1

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, 'operation_type', text="")

    def draw_label(self):
        # Use the label of `operation_type` as displayed node name
        for op_entry in self.operation_type_items:
            if self.operation_type in op_entry:
                return op_entry[1]
