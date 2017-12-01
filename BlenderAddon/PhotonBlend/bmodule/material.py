from ..utility import settings
from . import ui

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

		ui.material.display_blender_props(layout, material)

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

	ui.material.define_blender_props()

	for panel_type in material_panel_types:
		bpy.utils.register_class(panel_type)


def unregister():
	for panel_type in material_panel_types:
		bpy.utils.unregister_class(panel_type)


if __name__ == "__main__":
	register()
