from ..utility import settings
from . import ui

import sys
import bpy


class PhMaterialPanel(bpy.types.Panel):
	bl_space_type  = "PROPERTIES"
	bl_region_type = "WINDOW"
	bl_context     = "material"

	COMPATIBLE_ENGINES = {settings.renderer_id_name,
	                      settings.cycles_id_name}

	@classmethod
	def poll(cls, b_context):
		render_settings = b_context.scene.render
		return (render_settings.engine in cls.COMPATIBLE_ENGINES and
		        b_context.material)


class PhMainPropertyPanel(PhMaterialPanel):

	"""
	Setting up primary material properties.
	"""

	bl_label = "PR - Material"

	def draw(self, context):

		material = context.material
		layout   = self.layout

		ui.material.display_blender_props(layout, material)


class PhOptionPanel(PhMaterialPanel):

	"""
	Additional options for tweaking the material.
	"""

	bl_label = "PR - Options"

	bpy.types.Material.ph_is_emissive = bpy.props.BoolProperty(
		name        = "Emissive",
		description = "whether consider current material's emissivity or not",
		default     = False
	)

	bpy.types.Material.ph_emitted_radiance = bpy.props.FloatVectorProperty(
		name        = "Radiance",
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

		row = layout.row()
		row.prop(material, "ph_is_emissive")
		row.prop(material, "ph_emitted_radiance")


MATERIAL_PANEL_TYPES = [PhMainPropertyPanel, PhOptionPanel]


def register():

	ui.material.define_blender_props()

	for panel_type in MATERIAL_PANEL_TYPES:
		bpy.utils.register_class(panel_type)


def unregister():
	for panel_type in MATERIAL_PANEL_TYPES:
		bpy.utils.unregister_class(panel_type)


if __name__ == "__main__":
	register()
