from ..utility import settings
from ..psdl import lightcmd
from ..psdl import materialcmd
from ..psdl.cmd import RawCommand
from .export import naming
from .. import utility
from ..psdl import clause

import bpy
import mathutils

import sys
import math


class PhLightPanel(bpy.types.Panel):

	bl_space_type  = "PROPERTIES"
	bl_region_type = "WINDOW"
	bl_context     = "data"

	COMPATIBLE_ENGINES = {settings.renderer_id_name}

	@classmethod
	def poll(cls, context):
		render_settings = context.scene.render
		return (render_settings.engine in cls.COMPATIBLE_ENGINES and
		        context.lamp)


class PhLightPropertyPanel(PhLightPanel):

	"""
	Specify and control light properties.
	"""

	bl_label = "PR - Light"

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

	def draw(self, context):

		lamp   = context.lamp
		layout = self.layout

		# HACK: relying on blender lamp type to change lamp data
		layout.prop(lamp, "type", expand = True)

		layout.prop(lamp, "ph_light_color_linear_srgb")
		layout.prop(lamp, "ph_light_watts")

		if lamp.type == "AREA":

			split = layout.split()

			col = split.column()
			col.prop(lamp, "shape", text = "Shape")

			if lamp.shape == "SQUARE":

				col.prop(lamp, "size", text = "Size")

			elif lamp.shape == "RECTANGLE":

				col.prop(lamp, "size",   text = "Width")
				col.prop(lamp, "size_y", text = "Height")

			else:
				print("warning: unsupported area light shape %s" % lamp.shape)

		elif lamp.type == "POINT":

			# nothing to display
			pass

		else:
			print("warning: unsupported light type %s" % lamp.type)


def to_sdl_commands(b_obj, sdlconsole):

	b_lamp = b_obj.data

	source_name = naming.mangled_light_source_name(b_obj, b_lamp.name, "lamp_source")
	actor_name  = naming.mangled_actor_light_name(b_obj, b_lamp.name, "lamp_actor")

	if b_lamp.type == "AREA":

		# In Blender's Lamp, under Area category, only Square and Rectangle shape are available.
		# (which are both a rectangle in Photon)
		rec_width  = b_lamp.size
		rec_height = b_lamp.size_y if b_lamp.shape == "RECTANGLE" else b_lamp.size

		source_cmd = lightcmd.RectangleLightCreator()
		source_cmd.set_data_name(source_name)
		source_cmd.set_width(rec_width)
		source_cmd.set_height(rec_height)
		source_cmd.set_linear_srgb_color(b_lamp.ph_light_color_linear_srgb)
		source_cmd.set_watts(b_lamp.ph_light_watts)
		sdlconsole.queue_command(source_cmd)

	elif b_lamp.type == "POINT":

		source_cmd = lightcmd.PointLightCreator()
		source_cmd.set_data_name(source_name)
		source_cmd.set_linear_srgb_color(b_lamp.ph_light_color_linear_srgb)
		source_cmd.set_watts(b_lamp.ph_light_watts)
		sdlconsole.queue_command(source_cmd)

	else:
		print("warning: unsupported lamp type %s, ignoring" % b_lamp.type)
		return

	pos, rot, scale = b_obj.matrix_world.decompose()

	# Blender's rectangle area light is in its xy-plane (facing -z axis) by default,
	# while Photon's rectangle is in Blender's yz-plane (facing +x axis); these
	# rotations accounts for such difference
	rot = rot * mathutils.Quaternion((1.0, 0.0, 0.0), math.radians(90.0))
	rot = rot * mathutils.Quaternion((0.0, 0.0, 1.0), math.radians(-90.0))

	pos   = utility.to_photon_vec3(pos)
	rot   = utility.to_photon_quat(rot)
	scale = utility.to_photon_vec3(scale)

	actor_cmd = RawCommand()
	actor_cmd.append_string(
		"-> actor(light) @%s [light-source light-source @%s] \n" %
		(actor_name, source_name)
	)
	actor_cmd.append_string(
		"-> actor(light) translate(@%s) [vector3r factor \"%.8f %.8f %.8f\"] \n" %
		(actor_name, pos.x, pos.y, pos.z)
	)
	actor_cmd.append_string(
		"-> actor(light) rotate(@%s) [quaternionR factor \"%.8f %.8f %.8f %.8f\"] \n" %
		(actor_name, rot.x, rot.y, rot.z, rot.w)
	)
	actor_cmd.append_string(
		"-> actor(light) scale(@%s) [vector3r factor \"%.8f %.8f %.8f\"] \n" %
		(actor_name, scale.x, scale.y, scale.z)
	)
	sdlconsole.queue_command(actor_cmd)


LIGHT_PANEL_TYPES = [PhLightPropertyPanel]


def register():
	for panel_type in LIGHT_PANEL_TYPES:
		bpy.utils.register_class(panel_type)


def unregister():
	for panel_type in LIGHT_PANEL_TYPES:
		bpy.utils.unregister_class(panel_type)


if __name__ == "__main__":
	register()
