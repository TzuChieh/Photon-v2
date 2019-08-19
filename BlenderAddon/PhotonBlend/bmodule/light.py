from ..utility import settings
from ..psdl import lightcmd
from ..psdl import materialcmd
from ..psdl.cmd import RawCommand
from .export import naming
from .. import utility
from ..psdl import clause

from ..psdl.pysdl import (
	SDLReal,
	SDLVector3,
	SDLQuaternion,
	SDLString,
	SDLReference,
	SDLLightSource)

from ..psdl.pysdl import (
	LightActorCreator,
	SphereLightSourceCreator,
	PointLightSourceCreator,
	RectangleLightSourceCreator,
	LightActorTranslate,
	LightActorRotate,
	LightActorScale)

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
		        context.light)


class PhLightPropertyPanel(PhLightPanel):

	"""
	Specify and control light properties.
	"""

	bl_label = "PR - Light"

	bpy.types.Light.ph_light_color_linear_srgb = bpy.props.FloatVectorProperty(
		name        = "Color",
		description = "light color in linear sRGB",
		default     = [1.0, 1.0, 1.0],
		min         = 0.0,
		max         = 1.0,
		subtype     = "COLOR",
		size        = 3
	)

	bpy.types.Light.ph_light_watts = bpy.props.FloatProperty(
		name        = "Watts",
		description = "light energy in watts",
		default     = 100,
		min         = 0.0,
		max         = sys.float_info.max
	)

	def draw(self, context):

		light  = context.light
		layout = self.layout

		# HACK: relying on blender light type to change light data
		layout.prop(light, "type", expand = True)

		layout.prop(light, "ph_light_color_linear_srgb")
		layout.prop(light, "ph_light_watts")

		if light.type == "AREA":

			split = layout.split()

			col = split.column()
			col.prop(light, "shape", text = "Shape")

			if light.shape == "SQUARE":

				col.prop(light, "size", text = "Size")

			elif light.shape == "RECTANGLE":

				col.prop(light, "size",   text = "Width")
				col.prop(light, "size_y", text = "Height")

			else:
				print("warning: unsupported area light shape %s" % light.shape)

		elif light.type == "POINT":

			# nothing to display
			pass

		else:
			print("warning: unsupported light type %s" % light.type)


def to_sdl_commands(b_obj, sdlconsole):

	b_light = b_obj.data

	source_name = naming.mangled_light_source_name(b_obj, b_light.name, "bLight_source")
	actor_name  = naming.mangled_actor_light_name(b_obj, b_light.name, "bLight_actor")

	if b_light.type == "AREA":

		# In Blender's Light, under Area category, only Square and Rectangle shape are available.
		# (which are both a rectangle in Photon)
		rec_width  = b_light.size
		rec_height = b_light.size_y if b_light.shape == "RECTANGLE" else b_light.size

		creator = RectangleLightSourceCreator()
		creator.set_data_name(source_name)
		creator.set_width(SDLReal(rec_width))
		creator.set_height(SDLReal(rec_height))
		creator.set_linear_srgb(SDLVector3(b_light.ph_light_color_linear_srgb))
		creator.set_watts(SDLReal(b_light.ph_light_watts))
		sdlconsole.queue_command(creator)

	elif b_light.type == "POINT":

		creator = PointLightSourceCreator()
		creator.set_data_name(source_name)
		creator.set_linear_srgb(SDLVector3(b_light.ph_light_color_linear_srgb))
		creator.set_watts(SDLReal(b_light.ph_light_watts))
		sdlconsole.queue_command(creator)

	else:
		print("warning: unsupported light type %s, ignoring" % b_light.type)
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

	creator = LightActorCreator()
	creator.set_data_name(actor_name)
	creator.set_light_source(SDLLightSource(source_name))
	sdlconsole.queue_command(creator)

	translator = LightActorTranslate()
	translator.set_target_name(actor_name)
	translator.set_factor(SDLVector3(pos))
	sdlconsole.queue_command(translator)

	rotator = LightActorRotate()
	rotator.set_target_name(actor_name)
	rotator.set_factor(SDLQuaternion((rot.x, rot.y, rot.z, rot.w)))
	sdlconsole.queue_command(rotator)

	scaler = LightActorScale()
	scaler.set_target_name(actor_name)
	scaler.set_factor(SDLVector3(scale))
	sdlconsole.queue_command(scaler)


LIGHT_PANEL_TYPES = [PhLightPropertyPanel]


def register():
	for panel_type in LIGHT_PANEL_TYPES:
		bpy.utils.register_class(panel_type)


def unregister():
	for panel_type in LIGHT_PANEL_TYPES:
		bpy.utils.unregister_class(panel_type)


if __name__ == "__main__":
	register()
