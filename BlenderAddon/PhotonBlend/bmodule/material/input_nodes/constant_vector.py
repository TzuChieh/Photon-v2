from ..node_base import (
        PhMaterialInputNode,
        PhFloatVectorSocket)
from psdl import sdl
from ... import naming

import bpy


class PhConstantVectorInputNode(PhMaterialInputNode):
    bl_idname = 'PH_CONSTANT_VECTOR'
    bl_label = "Constant Vector"

    size: bpy.props.IntProperty(
        name="Size",
        description="Number of vector elements/dimensions.",
        default=3,
        min=1,
        max=8
    )

    values: bpy.props.FloatVectorProperty(
        name="Values",
        description="Numeric values.",
        default=[0] * 8,
        min=-1e32,
        max=1e32,
        subtype='NONE',
        size=8
    )

    def to_sdl(self, b_material, sdlconsole):
        output_socket = self.outputs[0]
        creator = sdl.ConstantImageCreator()
        creator.set_data_name(naming.get_mangled_output_node_socket_name(output_socket, b_material))
        creator.set_values(sdl.RealArray(self.values[:self.size]))
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.outputs.new(PhFloatVectorSocket.bl_idname, "")

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, 'size')
        for i in range(self.size):
            b_layout.prop(self, 'values', index=i, text="[%d]" % i)
