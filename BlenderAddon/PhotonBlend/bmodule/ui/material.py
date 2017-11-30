from ... import psdl

import bpy
import mathutils


class MaterialType(bpy.types.PropertyGroup):

	@classmethod
	def get_description(cls):
		return "Photon-v2's material"


class MatteOpaque(MaterialType):

	@classmethod
	def define_blender_props(cls):

		cls.albedo = bpy.props.FloatVectorProperty(
			name        = "albedo",
			description = "surface albedo in [0, 1]",
			default     = [0.5, 0.5, 0.5],
			min         = 0.0,
			max         = 1.0,
			subtype     = "COLOR",
			size        = 3
		)

	@classmethod
	def display_blender_props(cls, b_layout, b_prop_group):
		b_layout.prop(b_prop_group, "albedo")

	@classmethod
	def to_sdl(cls, b_prop_group, res_name):
		albedo       = b_prop_group.albedo
		albedo_color = mathutils.Color((albedo[0], albedo[1], albedo[2]))
		command      = psdl.materialcmd.MatteOpaque.create(res_name, albedo_color)
		return command.to_sdl()

	@classmethod
	def get_name(cls):
		return "Matte Opaque"


class AbradedOpaque(MaterialType):

	@classmethod
	def define_blender_props(cls):

		cls.albedo = bpy.props.FloatVectorProperty(
			name        = "albedo",
			description = "surface albedo in [0, 1]",
			default     = [0.5, 0.5, 0.5],
			min         = 0.0,
			max         = 1.0,
			subtype     = "COLOR",
			size        = 3
		)

		bpy.types.Material.roughness = bpy.props.FloatProperty(
			name        = "roughness",
			description = "surface roughness in [0, 1]",
			default     = 0.5,
			min         = 0.0,
			max         = 1.0
		)

		bpy.types.Material.roughness_u = bpy.props.FloatProperty(
			name        = "roughness u",
			description = "surface anisotropic roughness in [0, 1]",
			default     = 0.5,
			min         = 0.0,
			max         = 1.0
		)

		bpy.types.Material.roughness_v = bpy.props.FloatProperty(
			name        = "roughness v",
			description = "surface anisotropic roughness in [0, 1]",
			default     = 0.0,
			min         = 0.0,
			max         = 1.0
		)

		bpy.types.Material.f0 = bpy.props.FloatVectorProperty(
			name        = "F0",
			description = "surface reflectivity at normal incidence in [0, 1]",
			default     = [0.04, 0.04, 0.04],
			min         = 0.0,
			max         = 1.0,
			subtype     = "COLOR",
			size=3
		)

		bpy.types.Material.ph_isEmissive = bpy.props.BoolProperty(
			name="emissive",
			description="whether consider current material's emissivity or not",
			default=False
		)

		bpy.types.Material.ph_isAnisotropic = bpy.props.BoolProperty(
			name="anisotropic",
			description="does this material has anisotropic roughness",
			default=False
		)

	@classmethod
	def display_blender_props(cls, b_layout, b_prop_group):
		b_layout.prop(b_prop_group, "albedo")

	@classmethod
	def to_sdl(cls, b_prop_group, res_name):
		albedo = b_prop_group.albedo
		albedo_color = mathutils.Color((albedo[0], albedo[1], albedo[2]))
		command = psdl.materialcmd.MatteOpaque.create(res_name, albedo_color)
		return command.to_sdl()

	@classmethod
	def get_name(cls):
		return "Matte Opaque"


AVAILABLE_MATERIAL_TYPES = [MatteOpaque]


def define_blender_props():

	b_enum_items = []
	for material_type in AVAILABLE_MATERIAL_TYPES:
		mat_type    = material_type.__name__
		name        = material_type.get_name()
		description = material_type.get_description()
		b_enum_items.append((mat_type, name, description))

	bpy.types.Material.ph_material_type = bpy.props.EnumProperty(
		items       = b_enum_items,
		name        = "Material Type",
		description = "Photon-v2's supported material types",
		default     = b_enum_items[0][0]
	)

	for material_type in AVAILABLE_MATERIAL_TYPES:
		material_type.define_blender_props()
		bpy.utils.register_class(material_type)

		b_pointer_prop      = bpy.props.PointerProperty(type = material_type)
		b_pointer_prop_name = "ph_" + material_type.__name__
		setattr(bpy.types.Material, b_pointer_prop_name, b_pointer_prop)


def display_blender_props(b_layout, b_material):

	for material_type in AVAILABLE_MATERIAL_TYPES:
		if material_type.__name__ == b_material.ph_material_type:
			b_pointer_prop_name = "ph_" + material_type.__name__
			b_prop_group        = getattr(b_material, b_pointer_prop_name)
			material_type.display_blender_props(b_layout, b_prop_group)


def to_sdl(b_material, res_name):

	for material_type in AVAILABLE_MATERIAL_TYPES:
		if material_type.__name__ == b_material.ph_material_type:
			b_pointer_prop_name = "ph_" + material_type.__name__
			b_prop_group        = getattr(b_material, b_pointer_prop_name)
			return material_type.to_sdl(b_prop_group, res_name)

	print("warning: at to_sdl(), "
	      "SDL generating failed for material %s" % b_material.name)
	return ""
