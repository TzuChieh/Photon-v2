from ..node_base import (
    PhMaterialInputNode,
    PhFloatVectorSocket)
from psdl import sdl

import bpy


class PhAttributeInputNode(PhMaterialInputNode):
    bl_idname = 'PH_ATTRIBUTE'
    bl_label = "Attribute"

    kind: bpy.props.EnumProperty(
        items=[
            ('uvw-from-geometry-bound', "UVW From Geometry Bound", "Position normalized by the complete local geometry bounds", 0),
            ('geometry-hit-position', "Geometry Hit Position", "Unnormalized local-space hit position", 1),
        ],
        name="Kind",
        description="Surface attribute data to expose",
        default='uvw-from-geometry-bound'
    )

    def to_sdl(self, b_material, sdlconsole):
        creator = sdl.AttributeImageCreator()
        creator.set_data_name(self.get_output_resource_name(b_material))
        creator.set_kind(sdl.Enum(self.kind))
        sdlconsole.queue_command(creator)

    def init(self, b_context):
        self.outputs.new(PhFloatVectorSocket.bl_idname, PhFloatVectorSocket.bl_label)

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, 'kind', text="")
