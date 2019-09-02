from ..node_base import (
        PhMaterialNode,
        PhSurfaceLayerSocket,
        SURFACE_MATERIAL_CATEGORY)

import bpy

import sys


class PhSurfaceLayerNode(PhMaterialNode):
    bl_idname = "PH_SURFACE_LAYER"
    bl_label = "Surface Layer"
    node_category = SURFACE_MATERIAL_CATEGORY

    roughness: bpy.props.FloatProperty(
        name="Roughness",
        default=0.5,
        min=0.0,
        max=1.0
    )

    ior_type: bpy.props.EnumProperty(
        items=[
            ("SCALAR", "Scalar", ""),
            ("RGB", "RGB", "")
        ],
        name="IoR Type",
        description="Type of IoR data used.",
        default="SCALAR"
    )

    ior_n: bpy.props.FloatProperty(
        name="IoR N",
        default=1.5,
        min=0.0,
        max=sys.float_info.max
    )

    ior_k: bpy.props.FloatProperty(
        name="IoR K",
        default=0.0,
        min=0.0,
        max=sys.float_info.max
    )

    ior_n_rgb: bpy.props.FloatVectorProperty(
        name="IoR N",
        description="RGB value of IoR N.",
        default=[1.5, 1.5, 1.5],
        min=0.0,
        max=sys.float_info.max,
        subtype="COLOR",
        size=3
    )

    ior_k_rgb: bpy.props.FloatVectorProperty(
        name="IoR K",
        description="RGB value of IoR K.",
        default=[0.0, 0.0, 0.0],
        min=0.0,
        max=sys.float_info.max,
        subtype="COLOR",
        size=3
    )

    depth: bpy.props.FloatProperty(
        name="depth",
        default=0.0,
        min=0.0,
        max=sys.float_info.max
    )

    g: bpy.props.FloatProperty(
        name="g",
        default=0.9,
        min=0.5,
        max=1.0
    )

    sigma_a: bpy.props.FloatProperty(
        name="Sigma A",
        default=0.1,
        min=0.0,
        max=sys.float_info.max
    )

    sigma_s: bpy.props.FloatProperty(
        name="Sigma S",
        default=0.1,
        min=0.0,
        max=sys.float_info.max
    )

    def to_sdl(self, b_material, sdlconsole):
        pass

    def init(self, b_context):
        self.outputs.new(PhSurfaceLayerSocket.bl_idname, PhSurfaceLayerSocket.bl_label)

    def draw_buttons(self, b_context, b_layout):
        b_layout.prop(self, "ior_type")
        b_layout.prop(self, "roughness")

        if self.ior_type == "SCALAR":
            b_layout.prop(self, "ior_n")
            b_layout.prop(self, "ior_k")
        elif self.ior_type == "RGB":
            b_layout.prop(self, "ior_n_rgb")
            b_layout.prop(self, "ior_k_rgb")

        b_layout.prop(self, "depth")
        b_layout.prop(self, "g")
        b_layout.prop(self, "sigma_a")
        b_layout.prop(self, "sigma_s")
