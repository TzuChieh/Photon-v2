from ..utility import settings, blender

import bpy

import sys


class PH_CAMERA_PT_camera(bpy.types.Panel):
    bl_label = "PR: Camera"
    bl_context = "data"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"

    COMPATIBLE_ENGINES = {settings.renderer_id_name}

    bpy.types.Camera.ph_resolution_x = bpy.props.IntProperty(
        name="Resolution X",
        description="",
        default=1920,
        min=1
    )

    bpy.types.Camera.ph_resolution_y = bpy.props.IntProperty(
        name="Resolution Y",
        description="",
        default=1080,
        min=1
    )

    bpy.types.Camera.ph_resolution_percentage = bpy.props.FloatProperty(
        name="Resolution Scale",
        description="",
        subtype='PERCENTAGE',
        default=50,
        min=0,
        max=100
    )

    bpy.types.Camera.ph_has_dof = bpy.props.BoolProperty(
        name="Depth of Field",
        description="",
        default=False
    )

    bpy.types.Camera.ph_lens_radius_mm = bpy.props.FloatProperty(
        name="Lens Radius (millimeter)",
        description="",
        default=52,
        min=0,
        max=sys.float_info.max
    )

    bpy.types.Camera.ph_focal_meters = bpy.props.FloatProperty(
        name="Focal Distance (meter)",
        description="",
        default=3,
        min=0,
        max=sys.float_info.max
    )

    @classmethod
    def poll(cls, b_context):
        render_settings = b_context.scene.render
        return render_settings.engine in cls.COMPATIBLE_ENGINES and b_context.camera

    def draw(self, b_context):

        b_layout = self.layout
        b_camera = b_context.camera

        b_layout.prop(b_camera, "ph_resolution_x")
        b_layout.prop(b_camera, "ph_resolution_y")
        b_layout.prop(b_camera, "ph_resolution_percentage")

        b_layout.prop(b_camera, "ph_has_dof")

        dof_row = b_layout.row()
        dof_row.enabled = b_camera.ph_has_dof
        dof_row.prop(b_camera, "ph_lens_radius_mm")
        dof_row.prop(b_camera, "ph_focal_meters")


CAMERA_PANELS = [
    PH_CAMERA_PT_camera
]


def include_module(module_manager):
    for clazz in CAMERA_PANELS:
        module_manager.add_class(clazz)
