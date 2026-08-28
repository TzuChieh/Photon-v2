from ..node_base import (
    PhMaterialInputNode,
    PhFloatVectorSocket)
from bmodule.mesh import attributes
from psdl import sdl

import bpy


class PhAttributeInputNode(PhMaterialInputNode):
    bl_idname = 'PH_ATTRIBUTE'
    bl_label = "Attribute"

    type: bpy.props.EnumProperty(
        items=[
            (
                'uvw-from-geometry-bound',
                "UVW From Geometry Bound",
                "Position normalized by the complete local geometry bounds",
                0
            ),
            (
                'geometry-hit-position',
                "Geometry Hit Position",
                "Unnormalized local-space hit position",
                1
            ),
            (
                'random-per-island',
                "Random Per Island",
                "Cycles-compatible random value for each connected mesh component",
                2
            ),
        ],
        name="Type",
        description="Surface attribute data to expose",
        default='uvw-from-geometry-bound'
    )

    def to_sdl(self, b_material, sdlconsole, export_ctx):
        photon_kind = self.type
        if self.is_geometry_attribute():
            geometry_attribute = attributes.GeometryAttribute(self.type)
            custom_slot = export_ctx.geometry_attribute_to_custom_slot[geometry_attribute]
            photon_kind = f"custom-face-{custom_slot}"

        creator = sdl.AttributeImageCreator()
        creator.set_data_name(self.get_output_resource_name(b_material))
        creator.set_kind(sdl.Enum(photon_kind))
        sdlconsole.queue_command(creator)

    def used_geometry_attributes(self):
        if self.is_geometry_attribute():
            return (attributes.GeometryAttribute(self.type),)
        
        return ()

    def is_geometry_attribute(self):
        return self.type == attributes.GeometryAttribute.RANDOM_PER_ISLAND.value

    def init(self, b_context):
        self.outputs.new(PhFloatVectorSocket.bl_idname, PhFloatVectorSocket.bl_label)

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, 'type', text="")
