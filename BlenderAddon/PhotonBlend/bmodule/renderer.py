from ..utility import settings

import bpy
from bl_ui import (
		properties_render,
		properties_data_camera,
		#properties_data_lamp,
		#properties_material,
)


class PhotonRenderer(bpy.types.RenderEngine):

	# These three members are used by blender to set up the
	# RenderEngine; define its internal name, visible name and capabilities.
	bl_idname      = settings.renderer_id_name
	bl_label       = "Photon"
	bl_use_preview = False

	# This is the only method called by blender.
	def render(self, scene):
		pass


class PhRenderPanel(bpy.types.Panel):

	bl_space_type  = "PROPERTIES"
	bl_region_type = "WINDOW"
	bl_context     = "render"

	COMPATIBLE_ENGINES = {settings.renderer_id_name}

	@classmethod
	def poll(cls, context):
		render_settings = context.scene.render
		return render_settings.engine in cls.COMPATIBLE_ENGINES


class PhRenderingPanel(PhRenderPanel):

	bl_label = "PR - Rendering"

	bpy.types.Scene.ph_render_integrator_type = bpy.props.EnumProperty(
		items = [
			("BVPT",   "Pure Path Tracing",  "slow but unbiased"),
			("BNEEPT", "NEE Path Tracing",   "fairly slow but good on rendering small lights")
		],
		name        = "Rendering Method",
		description = "Photon-v2's rendering methods",
		default     = "BNEEPT"
	)

	def draw(self, context):

		scene  = context.scene
		layout = self.layout

		layout.prop(scene, "ph_render_integrator_type")


class PhSamplingPanel(PhRenderPanel):

	bl_label = "PR - Sampling"

	bpy.types.Scene.ph_render_num_spp = bpy.props.IntProperty(
		name        = "Samples per Pixel",
		description = "Number of samples used for each pixel.",
		default     = 40,
		min         = 1,
		max         = 2**31 - 1,
	)

	bpy.types.Scene.ph_render_sample_filter_type = bpy.props.EnumProperty(
		items = [
			("BOX",      "Box",                "box filter"),
			("GAUSSIAN", "Gaussian",           "Gaussian filter"),
			("MN",       "Mitchell-Netravali", "Mitchell-Netravali filter")
		],
		name        = "Sample Filter Type",
		description = "Photon-v2's sample filter types",
		default     = "MN"
	)

	def draw(self, context):

		scene  = context.scene
		layout = self.layout

		layout.prop(scene, "ph_render_num_spp")
		layout.prop(scene, "ph_render_sample_filter_type")


class PhOptionsPanel(PhRenderPanel):

	bl_label = "PR - Options"

	bpy.types.Scene.ph_use_cycles_material = bpy.props.BoolProperty(
		name        = "Use Cycles Material",
		description = "render/export the scene with materials converted from Cycles to Photon",
		default     = False
	)

	def draw(self, context):

		scene  = context.scene
		layout = self.layout

		layout.prop(scene, "ph_use_cycles_material")


render_panel_types = [
	PhSamplingPanel,
	PhOptionsPanel,
	PhRenderingPanel
]


def register():
	# Register the RenderEngine.
	bpy.utils.register_class(PhotonRenderer)

	# RenderEngines also need to tell UI Panels that they are compatible;
	# otherwise most of the UI will be empty when the engine is selected.


	properties_render.RENDER_PT_dimensions.COMPAT_ENGINES.add(PhotonRenderer.bl_idname)

	properties_data_camera.DATA_PT_lens.COMPAT_ENGINES.add(PhotonRenderer.bl_idname)
	properties_data_camera.DATA_PT_camera.COMPAT_ENGINES.add(PhotonRenderer.bl_idname)

	#properties_data_lamp.DATA_PT_lamp.COMPAT_ENGINES.add(PhotonRenderer.bl_idname)
	#properties_data_lamp.DATA_PT_area.COMPAT_ENGINES.add(PhotonRenderer.bl_idname)

	#properties_material.MATERIAL_PT_preview.COMPAT_ENGINES.add(PhotonRenderer.bl_idname)

	for panel_type in render_panel_types:
		bpy.utils.register_class(panel_type)


def unregister():
	bpy.utils.unregister_class(PhotonRenderer)

	properties_render.RENDER_PT_render.COMPAT_ENGINES.remove(PhotonRenderer.bl_idname)

	properties_data_camera.DATA_PT_lens.COMPAT_ENGINES.remove(PhotonRenderer.bl_idname)
	properties_data_camera.DATA_PT_camera.COMPAT_ENGINES.remove(PhotonRenderer.bl_idname)

	#properties_data_lamp.DATA_PT_lamp.COMPAT_ENGINES.remove(PhotonRenderer.bl_idname)
	#properties_data_lamp.DATA_PT_area.COMPAT_ENGINES.remove(PhotonRenderer.bl_idname)

	#properties_material.MATERIAL_PT_preview.COMPAT_ENGINES.remove(PhotonRenderer.bl_idname)

	for panel_type in render_panel_types:
		bpy.utils.unregister_class(panel_type)
