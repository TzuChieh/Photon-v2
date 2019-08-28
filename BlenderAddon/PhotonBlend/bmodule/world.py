from ..utility import settings

import bpy


class PhWorldPanel(bpy.types.Panel):
	bl_label       = "PR: Environment Map"
	bl_context     = "world"
	bl_space_type  = "PROPERTIES"
	bl_region_type = "WINDOW"

	COMPATIBLE_ENGINES = {settings.renderer_id_name}

	bpy.types.World.ph_envmap_file_path = bpy.props.StringProperty(
		name    = "File",
		default = "",
		subtype = "FILE_PATH"
	)

	bpy.types.World.ph_envmap_degrees = bpy.props.FloatProperty(
		name        = "Y Rotation Degrees",
		description = "",
		default     = 0,
		min         = 0,
		max         = 360
	)

	@classmethod
	def poll(cls, b_context):
		render_settings = b_context.scene.render
		return render_settings.engine in cls.COMPATIBLE_ENGINES and b_context.world

	def draw(self, b_context):

		b_layout = self.layout
		b_world  = b_context.world

		b_layout.prop(b_world, "ph_envmap_file_path")
		b_layout.prop(b_world, "ph_envmap_degrees")


WORLD_PANEL_CLASSES = [
	PhWorldPanel
]


def include_module(module_manager):
	for clazz in WORLD_PANEL_CLASSES:
		module_manager.add_class(clazz)
