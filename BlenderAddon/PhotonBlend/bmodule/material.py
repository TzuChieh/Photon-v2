from ..utility import settings

import sys
import bpy


class PhMaterialPanel:
	bl_space_type  = "PROPERTIES"
	bl_region_type = "WINDOW"
	bl_context     = "material"

	COMPATIBLE_ENGINES = {settings.renderer_id_name,
	                      settings.cycles_id_name}

	@classmethod
	def poll(cls, context):
		render_settings = context.scene.render
		return render_settings.engine in cls.COMPATIBLE_ENGINES


class PhMaterial(bpy.types.PropertyGroup):
	pass


class PhGeneralPanel(PhMaterialPanel, bpy.types.Panel):
	"""Photon's material panel"""
	bl_label = "Photon-v2 Material"

	bpy.types.Material.ph_materialType = bpy.props.EnumProperty(
		items = [
			("MATTE_OPAQUE",        "Matte Opaque",        "diffuse material"),
			("ABRADED_OPAQUE",      "Abraded Opaque",      "microfacet material"),
			("ABRADED_TRANSLUCENT", "Abraded Translucent", "microfacet dielectric material")
		],
		name        = "Material Type",
		description = "Photon-v2's material types",
		default     = "MATTE_OPAQUE"
	)

	bpy.types.Material.ph_albedo = bpy.props.FloatVectorProperty(
		name        = "albedo",
		description = "surface albedo in [0, 1]",
		default     = [0.5, 0.5, 0.5],
		min         = 0.0,
		max         = 1.0,
		subtype     = "COLOR",
		size        = 3
	)

	bpy.types.Material.ph_roughness = bpy.props.FloatProperty(
		name        = "roughness",
		description = "surface roughness in [0, 1]",
		default     = 0.5,
		min         = 0.0,
		max         = 1.0
	)

	bpy.types.Material.ph_ior = bpy.props.FloatProperty(
		name        = "index of refraction",
		description = "index of refraction of the material in [0, infinity]",
		default     = 1.5,
		min         = 0.0,
		max         = sys.float_info.max
	)

	bpy.types.Material.ph_f0 = bpy.props.FloatVectorProperty(
		name        = "F0",
		description = "surface reflectivity at normal incidence in [0, 1]",
		default     = [0.04, 0.04, 0.04],
		min         = 0.0,
		max         = 1.0,
		subtype     = "COLOR",
		size        = 3
	)

	bpy.types.Material.ph_isEmissive = bpy.props.BoolProperty(
		name        = "emissive",
		description = "whether consider current material's emissivity or not",
		default     = False
	)

	bpy.types.Material.ph_emittedRadiance = bpy.props.FloatVectorProperty(
		name        = "radiance",
		description = "radiance emitted by the surface",
		default     = [0.0, 0.0, 0.0],
		min         = 0.0,
		max         = sys.float_info.max,
		subtype     = "COLOR",
		size        = 3
	)

	def draw(self, context):
		material = context.material
		layout   = self.layout

		if material is None:
			layout.label("no material for Photon-v2 material panel")
			return

		layout.prop(material, "ph_materialType")
		materialType = material.ph_materialType

		if materialType == "MATTE_OPAQUE":

			layout.prop(material, "ph_albedo")

		elif materialType == "ABRADED_OPAQUE":

			layout.prop(material, "ph_albedo")
			layout.prop(material, "ph_f0")
			layout.prop(material, "ph_roughness")

		elif materialType == "ABRADED_TRANSLUCENT":

			layout.prop(material, "ph_albedo")
			layout.prop(material, "ph_f0")
			layout.prop(material, "ph_roughness")
			layout.prop(material, "ph_ior")

		else:
			print("warning: unknown type of Photon-v2 material (%s)" %(materialType))

		row = layout.row()
		row.prop(material, "ph_isEmissive")
		row.prop(material, "ph_emittedRadiance")


# class PhSettingsPanel(PhMaterialPanel, bpy.types.Panel):
# 	""" A panel for some general material settings. """
# 	bl_label = "Settings"
#
# 	def draw(self, context):
# 		layout = self.layout


material_panel_types = [PhGeneralPanel]


def register():
	for panel_type in material_panel_types:
		bpy.utils.register_class(panel_type)


def unregister():
	for panel_type in material_panel_types:
		bpy.utils.unregister_class(panel_type)


if __name__ == "__main__":
	register()
