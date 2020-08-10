from ..utility import settings

import bpy


class PH_WORLD_PT_background(bpy.types.Panel):
	bl_label = "PR: Background"
	bl_context = "world"
	bl_space_type = "PROPERTIES"
	bl_region_type = "WINDOW"

	COMPATIBLE_ENGINES = {settings.renderer_id_name}

	bpy.types.World.ph_background_type = bpy.props.EnumProperty(
		items=[
			('IMAGE', "Image", ""),
			('PREETHAM', "Preetham", "")
		],
		name="Background Type",
		description="Type of the background.",
		default='IMAGE'
	)

	bpy.types.World.ph_up_rotation = bpy.props.FloatProperty(
		name="Up Rotation",
		description="Up-axis rotation in degrees.",
		default=0,
		min=0,
		max=360
	)

	bpy.types.World.ph_image_file_path = bpy.props.StringProperty(
		name="File",
		default="",
		subtype="FILE_PATH"
	)

	bpy.types.World.ph_preetham_turbidity = bpy.props.FloatProperty(
		name="Turbidity",
		description="",
		default=2,
		min=1,
		max=64
	)

	bpy.types.World.ph_standard_time = bpy.props.FloatProperty(
		name="Standard Time",
		description="Standard time in 24H.",
		default=12,
		min=0,
		max=24
	)

	bpy.types.World.ph_standard_meridian = bpy.props.FloatProperty(
		name="Standard Meridian",
		description="Standard meridian in degrees.",
		default=0,
		min=-180,
		max=180
	)

	bpy.types.World.ph_latitude = bpy.props.FloatProperty(
		name="Latitude",
		description="Site latitude.",
		default=0,
		min=-90,
		max=90
	)

	bpy.types.World.ph_longitude = bpy.props.FloatProperty(
		name="Longitude",
		description="Site longitude.",
		default=0,
		min=-180,
		max=180
	)

	bpy.types.World.ph_julian_date = bpy.props.IntProperty(
		name="Julian Date",
		description="Julian date.",
		default=1,
		min=1,
		max=366
	)

	bpy.types.World.ph_energy_scale = bpy.props.FloatProperty(
		name="Energy Scale",
		description="A non-physical scale factor for artistic purpose.",
		default=1.0
	)

	@classmethod
	def poll(cls, b_context):
		render_settings = b_context.scene.render
		return render_settings.engine in cls.COMPATIBLE_ENGINES and b_context.world

	def draw(self, b_context):
		b_layout = self.layout
		b_world = b_context.world

		b_layout.prop(b_world, "ph_background_type")
		b_layout.prop(b_world, "ph_up_rotation")

		if b_world.ph_background_type == 'IMAGE':
			b_layout.prop(b_world, "ph_image_file_path")
		elif b_world.ph_background_type == 'PREETHAM':
			b_layout.prop(b_world, "ph_preetham_turbidity")
			b_layout.prop(b_world, "ph_standard_time")
			b_layout.prop(b_world, "ph_standard_meridian")
			b_layout.prop(b_world, "ph_latitude")
			b_layout.prop(b_world, "ph_longitude")
			b_layout.prop(b_world, "ph_julian_date")

		b_layout.prop(b_world, "ph_energy_scale")


WORLD_PANEL_CLASSES = [
	PH_WORLD_PT_background
]


def include_module(module_manager):
	for clazz in WORLD_PANEL_CLASSES:
		module_manager.add_class(clazz)
