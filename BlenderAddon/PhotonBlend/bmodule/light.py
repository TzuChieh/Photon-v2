from ..utility import settings

import sys
import bpy


class PhLightPanel(bpy.types.Panel):
	bl_space_type  = "PROPERTIES"
	bl_region_type = "WINDOW"
	bl_context     = "data"

	COMPATIBLE_ENGINES = {settings.renderer_id_name}

	@classmethod
	def poll(cls, context):
		render_settings = context.scene.render
		return render_settings.engine in cls.COMPATIBLE_ENGINES


# HACK: brings up lamp manipulating widgets by modifying blender's lamp data
def modify_blender_lamp_type_for_viewport_widgets(self, context):

	lamp = context.lamp

	if lamp.ph_light_type == "AREA":

		lamp.type = "AREA"

		if lamp.ph_area_light_shape_type == "SQUARE":

			lamp.shape = "SQUARE"
			lamp.size  = lamp.ph_area_light_square_size

		elif lamp.ph_area_light_shape_type == "RECTANGLE":

			lamp.shape = "RECTANGLE"
			lamp.lamp  = lamp.ph_area_light_rectangle_width
			lamp.lamp  = lamp.ph_area_light_rectangle_height

		else:
			lamp.shape = "SQUARE"

	elif lamp.ph_light_type == "POINT":

		lamp.type = "POINT"

	else:
		lamp.type = "POINT"


class PhLightPropertyPanel(PhLightPanel):

	"""
	Specify and control light properties.
	"""

	bl_label = "PR - Light"

	bpy.types.Lamp.ph_light_type = bpy.props.EnumProperty(
		items = [
			("AREA",  "Area",  "A finite area that is emitting energy."),
			("POINT", "Point", "An energy-emitting spot.")
		],
		name        = "Type",
		description = "Photon-v2's supported light types.",
		default     = "POINT",
		update      = modify_blender_lamp_type_for_viewport_widgets
	)

	bpy.types.Lamp.ph_light_color_linear_srgb = bpy.props.FloatVectorProperty(
		name        = "Color",
		description = "light color in linear sRGB",
		default     = [1.0, 1.0, 1.0],
		min         = 0.0,
		max         = 1.0,
		subtype     = "COLOR",
		size        = 3
	)

	bpy.types.Lamp.ph_light_watts = bpy.props.FloatProperty(
		name        = "Watts",
		description = "light energy in watts",
		default     = 100,
		min         = 0.0,
		max         = sys.float_info.max
	)

	bpy.types.Lamp.ph_area_light_shape_type = bpy.props.EnumProperty(
		items=[
			("SQUARE",    "Square",    "A suqare light shape."),
			("RECTANGLE", "Rectangle", "A rectangle light shape.")
		],
		name        = "Shape",
		description = "shape for an area light",
		default     = "SQUARE"
	)

	bpy.types.Lamp.ph_area_light_square_size = bpy.props.FloatProperty(
		name        = "Size",
		description = "Size of the square's side.",
		default     = 1,
		min         = 0.0,
		max         = sys.float_info.max
	)

	bpy.types.Lamp.ph_area_light_rectangle_width = bpy.props.FloatProperty(
		name        = "Width",
		description = "Width of the rectangle.",
		default     = 1,
		min         = 0.0,
		max         = sys.float_info.max
	)

	bpy.types.Lamp.ph_area_light_rectangle_height = bpy.props.FloatProperty(
		name        = "Height",
		description = "Height of the rectangle.",
		default     = 1,
		min         = 0.0,
		max         = sys.float_info.max
	)

	def draw(self, context):

		lamp   = context.lamp
		layout = self.layout

		layout.prop(lamp, "ph_light_type", expand = True)

		layout.prop(lamp, "ph_light_color_linear_srgb")
		layout.prop(lamp, "ph_light_watts")

		if lamp.ph_light_type == "AREA":

			split = layout.split()

			col = split.column()
			col.prop(lamp, "ph_area_light_shape_type")

			if lamp.ph_area_light_shape_type == "SQUARE":

				col.prop(lamp, "ph_area_light_square_size")

			elif lamp.ph_area_light_shape_type == "RECTANGLE":

				col.prop(lamp, "ph_area_light_rectangle_width")
				col.prop(lamp, "ph_area_light_rectangle_height")

			else:
				print("warning: unsupported area light shape %s" % lamp.ph_area_light_shape_type)

		elif lamp.ph_light_type == "POINT":

			# nothing to display
			pass

		else:
			print("warning: unsupported light type %s" % lamp.ph_light_type)


LIGHT_PANEL_TYPES = [PhLightPropertyPanel]


def register():
	for panel_type in LIGHT_PANEL_TYPES:
		bpy.utils.register_class(panel_type)


def unregister():
	for panel_type in LIGHT_PANEL_TYPES:
		bpy.utils.unregister_class(panel_type)


if __name__ == "__main__":
	register()
