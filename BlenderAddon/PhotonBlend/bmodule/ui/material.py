from ... import psdl

import bpy
import mathutils

import sys


class MaterialProperty:

	@classmethod
	def define_blender_props(cls, b_prop_group):
		pass

	@classmethod
	def display_blender_props(cls, b_layout, b_prop_group):
		pass


class MicrofacetProperty(MaterialProperty):

	@classmethod
	def define_blender_props(cls, b_prop_group):

		roughness = bpy.props.FloatProperty(
			name        = "Roughness",
			description = "surface roughness in [0, 1]",
			default     = 0.5,
			min         = 0.0,
			max         = 1.0
		)
		setattr(b_prop_group, "roughness", roughness)

		roughness_u = bpy.props.FloatProperty(
			name        = "Roughness U",
			description = "surface anisotropic roughness in [0, 1]",
			default     = 0.5,
			min         = 0.0,
			max         = 1.0
		)
		setattr(b_prop_group, "roughness_u", roughness_u)

		roughness_v = bpy.props.FloatProperty(
			name        = "Roughness V",
			description = "surface anisotropic roughness in [0, 1]",
			default     = 0.0,
			min         = 0.0,
			max         = 1.0
		)
		setattr(b_prop_group, "roughness_v", roughness_v)

		is_anisotropic = bpy.props.BoolProperty(
			name        = "Anisotropic",
			description = "does this material has anisotropic roughness",
			default     = False
		)
		setattr(b_prop_group, "is_anisotropic", is_anisotropic)

	@classmethod
	def display_blender_props(cls, b_layout, b_prop_group):

		if not b_prop_group.is_anisotropic:
			b_layout.prop(b_prop_group, "roughness")
		else:
			b_layout.prop(b_prop_group, "roughness_u")
			b_layout.prop(b_prop_group, "roughness_v")

	@classmethod
	def get_roughness(cls, b_prop_group):
		return b_prop_group.roughness

	@classmethod
	def get_roughness_u(cls, b_prop_group):
		return b_prop_group.roughness_u

	@classmethod
	def get_roughness_v(cls, b_prop_group):
		return b_prop_group.roughness_v

	@classmethod
	def is_anisotropic(cls, b_prop_group):
		return b_prop_group.is_anisotropic


class MaterialType(bpy.types.PropertyGroup):

	@classmethod
	def define_blender_props(cls):
		pass

	@classmethod
	def display_blender_props(cls, b_layout, b_prop_group):
		pass

	@classmethod
	def get_description(cls):
		return "Photon-v2's material"

	@classmethod
	def to_sdl(cls, b_prop_group, sdlconsole, res_name):
		pass

	@classmethod
	def get_name(cls):
		return ""


class MatteOpaque(MaterialType):

	@classmethod
	def define_blender_props(cls):

		cls.albedo = bpy.props.FloatVectorProperty(
			name        = "Albedo",
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
	def to_sdl(cls, b_prop_group, sdlconsole, res_name):

		albedo       = b_prop_group.albedo
		albedo_color = mathutils.Color((albedo[0], albedo[1], albedo[2]))

		command = psdl.materialcmd.MatteOpaqueCreator()
		command.set_data_name(res_name)
		command.set_albedo_color(albedo_color)

		return command.to_sdl(sdlconsole)

	@classmethod
	def get_name(cls):
		return "Matte Opaque"


class AbradedOpaque(MaterialType):

	@classmethod
	def define_blender_props(cls):

		MicrofacetProperty.define_blender_props(cls)

		cls.albedo = bpy.props.FloatVectorProperty(
			name        = "Albedo",
			description = "surface albedo in [0, 1]",
			default     = [0.5, 0.5, 0.5],
			min         = 0.0,
			max         = 1.0,
			subtype     = "COLOR",
			size        = 3
		)

		cls.f0 = bpy.props.FloatVectorProperty(
			name        = "F0",
			description = "surface reflectivity at normal incidence in [0, 1]",
			default     = [0.04, 0.04, 0.04],
			min         = 0.0,
			max         = 1.0,
			subtype     = "COLOR",
			size        = 3
		)

	@classmethod
	def display_blender_props(cls, b_layout, b_prop_group):

		b_layout.prop(b_prop_group, "albedo")
		b_layout.prop(b_prop_group, "f0")

		MicrofacetProperty.display_blender_props(b_layout, b_prop_group)

	@classmethod
	def to_sdl(cls, b_prop_group, sdlconsole, res_name):

		albedo_vec = b_prop_group.albedo
		albedo     = mathutils.Color((albedo_vec[0], albedo_vec[1], albedo_vec[2]))
		f0_vec     = b_prop_group.f0
		f0         = mathutils.Color((f0_vec[0], f0_vec[1], f0_vec[2]))

		command = psdl.materialcmd.AbradedOpaqueCreator()
		command.set_data_name(res_name)
		command.set_albedo(albedo)
		command.set_f0(f0)
		command.set_roughness(MicrofacetProperty.get_roughness(b_prop_group))
		command.set_anisotropicity(MicrofacetProperty.is_anisotropic(b_prop_group))
		command.set_roughness_u(MicrofacetProperty.get_roughness_u(b_prop_group))
		command.set_roughness_v(MicrofacetProperty.get_roughness_v(b_prop_group))

		return command.to_sdl(sdlconsole)

	@classmethod
	def get_name(cls):
		return "Abraded Opaque"


class AbradedTranslucent(MaterialType):

	@classmethod
	def define_blender_props(cls):

		MicrofacetProperty.define_blender_props(cls)

		cls.albedo = bpy.props.FloatVectorProperty(
			name        = "Albedo",
			description = "surface albedo in [0, 1]",
			default     = [0.5, 0.5, 0.5],
			min         = 0.0,
			max         = 1.0,
			subtype     = "COLOR",
			size        = 3
		)

		cls.f0 = bpy.props.FloatVectorProperty(
			name        = "F0",
			description = "surface reflectivity at normal incidence in [0, 1]",
			default     = [0.04, 0.04, 0.04],
			min         = 0.0,
			max         = 1.0,
			subtype     = "COLOR",
			size        = 3
		)

		cls.ior = bpy.props.FloatProperty(
			name        = "Index of Refraction",
			description = "index of refraction of the material in [0, infinity]",
			default     = 1.5,
			min         = 0.0,
			max         = sys.float_info.max
		)

	@classmethod
	def display_blender_props(cls, b_layout, b_prop_group):

		b_layout.prop(b_prop_group, "albedo")
		b_layout.prop(b_prop_group, "f0")
		b_layout.prop(b_prop_group, "ior")

		MicrofacetProperty.display_blender_props(b_layout, b_prop_group)

	@classmethod
	def to_sdl(cls, b_prop_group, sdlconsole, res_name):

		albedo_vec = b_prop_group.albedo
		albedo     = mathutils.Color((albedo_vec[0], albedo_vec[1], albedo_vec[2]))
		f0_vec     = b_prop_group.f0
		f0         = mathutils.Color((f0_vec[0], f0_vec[1], f0_vec[2]))

		command = psdl.materialcmd.AbradedTranslucentCreator()
		command.set_data_name(res_name)
		command.set_albedo(albedo)
		command.set_f0(f0)
		command.set_ior_outer(b_prop_group.ior)
		command.set_roughness(MicrofacetProperty.get_roughness(b_prop_group))

		return command.to_sdl(sdlconsole)

	@classmethod
	def get_name(cls):
		return "Abraded Translucent"


AVAILABLE_MATERIAL_TYPES = [MatteOpaque, AbradedOpaque, AbradedTranslucent]


def define_blender_props():

	b_enum_items = []
	for material_type in AVAILABLE_MATERIAL_TYPES:
		mat_type    = material_type.__name__
		name        = material_type.get_name()
		description = material_type.get_description()
		b_enum_items.append((mat_type, name, description))

	bpy.types.Material.ph_material_type = bpy.props.EnumProperty(
		items       = b_enum_items,
		name        = "Type",
		description = "Photon-v2's supported material types",
		default     = b_enum_items[0][0]
	)

	bpy.types.Material.ph_node_tree_name = bpy.props.StringProperty(
		name    = "Photon Node Tree Name",
		default = ""
	)

	for material_type in AVAILABLE_MATERIAL_TYPES:
		material_type.define_blender_props()
		bpy.utils.register_class(material_type)

		b_pointer_prop      = bpy.props.PointerProperty(type = material_type)
		b_pointer_prop_name = "ph_" + material_type.__name__
		setattr(bpy.types.Material, b_pointer_prop_name, b_pointer_prop)


def display_blender_props(b_layout, b_material):

	b_layout.prop(b_material, "ph_material_type")

	for material_type in AVAILABLE_MATERIAL_TYPES:
		if material_type.__name__ == b_material.ph_material_type:
			b_pointer_prop_name = "ph_" + material_type.__name__
			b_prop_group        = getattr(b_material, b_pointer_prop_name)
			material_type.display_blender_props(b_layout, b_prop_group)


def to_sdl(b_material, sdlconsole, res_name):

	for material_type in AVAILABLE_MATERIAL_TYPES:
		if material_type.__name__ == b_material.ph_material_type:
			b_pointer_prop_name = "ph_" + material_type.__name__
			b_prop_group        = getattr(b_material, b_pointer_prop_name)
			return material_type.to_sdl(b_prop_group, sdlconsole, res_name)

	print("warning: at to_sdl(), "
	      "SDL generating failed for material %s" % b_material.name)
	return ""
