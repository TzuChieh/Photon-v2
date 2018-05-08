from ..utility import settings

import bpy


class PhWorldPanel(bpy.types.Panel):
	bl_label       = "Environment Map"
	bl_context     = "world"
	bl_space_type  = "PROPERTIES"
	bl_region_type = "WINDOW"

	COMPATIBLE_ENGINES = {settings.renderer_id_name}

	bpy.types.World.ph_envmap_file_path = bpy.props.StringProperty(
		name    = "File",
		default = "",
		subtype = "FILE_PATH"
	)

	@classmethod
	def poll(cls, b_context):
		render_settings = b_context.scene.render
		return render_settings.engine in cls.COMPATIBLE_ENGINES and b_context.world

	def draw(self, b_context):
		b_layout = self.layout
		b_world  = b_context.world
		b_layout.prop(b_world, "ph_envmap_file_path")


WORLD_PANEL_TYPES = [
	PhWorldPanel
]


def register():

	class_types = WORLD_PANEL_TYPES
	for class_type in class_types:
		bpy.utils.register_class(class_type)


def unregister():

	class_types = WORLD_PANEL_TYPES
	for class_type in class_types:
		bpy.utils.unregister_class(class_type)
