from ..node_base import (
        PhMaterialNode,
        PhColorSocket,
        MATH_CATEGORY)
from psdl import sdl
from ... import naming

import bpy


class PhAddNode(PhMaterialNode):
    bl_idname = 'PH_ADD'
    bl_label = "Add"
    node_category = MATH_CATEGORY

    factor: bpy.props.FloatProperty(
        name="Factor",
        default=0.0,
        min=-1e32,
        max=1e32
    )

    def to_sdl(self, b_material, sdlconsole):
        input_color_socket = self.inputs[0]
        output_color_socket = self.outputs[0]
        input_color_res_name = input_color_socket.get_from_res_name(b_material)
        output_color_res_name = naming.get_mangled_output_node_socket_name(output_color_socket, b_material)
        if input_color_res_name is None:
            print("warning: node <%s> has no input linked, ignoring" % self.name)
            return

        creator = sdl.MathImageCreator()
        creator.set_data_name(output_color_res_name)
        creator.set_operand(sdl.Image(input_color_res_name))
        creator.set_math_image_op(sdl.Enum("ADD"))
        creator.set_scalar_input(sdl.Real(self.factor))
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.inputs.new(PhColorSocket.bl_idname, PhColorSocket.bl_label)
        self.outputs.new(PhColorSocket.bl_idname, PhColorSocket.bl_label)

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, "factor")
