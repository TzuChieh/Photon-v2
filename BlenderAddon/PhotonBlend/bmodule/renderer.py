from ..utility import settings

import bpy


class PhotonRenderer(bpy.types.RenderEngine):
	# These three members are used by blender to set up the
	# RenderEngine; define its internal name, visible name and capabilities.
	bl_idname      = settings.renderer_id_name
	bl_label       = "Photon"
	bl_use_preview = False

	# This is the only method called by blender.
	def render(self, scene):
		pass


class PhRenderPanel:
	bl_space_type  = "PROPERTIES"
	bl_region_type = "WINDOW"
	bl_context     = "render"

	COMPATIBLE_ENGINES = {settings.renderer_id_name}

	@classmethod
	def poll(cls, context):
		render_settings = context.scene.render
		return render_settings.engine in cls.COMPATIBLE_ENGINES


class PhSamplingPanel(PhRenderPanel, bpy.types.Panel):
	bl_label = "Sampling"

	bpy.types.Scene.ph_render_num_spp = bpy.props.IntProperty(
		name        = "Samples per Pixel",
		description = "Number of samples used for each pixel.",
		default     = 4,
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

render_panel_types = [PhSamplingPanel]


def register():
	# Register the RenderEngine.
	bpy.utils.register_class(PhotonRenderer)

	# RenderEngines also need to tell UI Panels that they are compatible
	# Otherwise most of the UI will be empty when the engine is selected.

	from bl_ui import (
		properties_render,
		properties_data_camera,
		properties_data_lamp,
		#properties_material,
	)
	properties_render.RENDER_PT_dimensions.COMPAT_ENGINES.add(PhotonRenderer.bl_idname)

	properties_data_camera.DATA_PT_lens.COMPAT_ENGINES.add(PhotonRenderer.bl_idname)
	properties_data_camera.DATA_PT_camera.COMPAT_ENGINES.add(PhotonRenderer.bl_idname)

	properties_data_lamp.DATA_PT_lamp.COMPAT_ENGINES.add(PhotonRenderer.bl_idname)
	properties_data_lamp.DATA_PT_area.COMPAT_ENGINES.add(PhotonRenderer.bl_idname)

	#properties_material.MATERIAL_PT_preview.COMPAT_ENGINES.add(PhotonRenderer.bl_idname)

	for panel_type in render_panel_types:
		bpy.utils.register_class(panel_type)


def unregister():
	bpy.utils.unregister_class(PhotonRenderer)

	from bl_ui import (
		properties_render,
		properties_data_camera,
		properties_data_lamp,
		#properties_material,
	)

	properties_render.RENDER_PT_render.COMPAT_ENGINES.remove(PhotonRenderer.bl_idname)

	properties_data_camera.DATA_PT_lens.COMPAT_ENGINES.remove(PhotonRenderer.bl_idname)
	properties_data_camera.DATA_PT_camera.COMPAT_ENGINES.remove(PhotonRenderer.bl_idname)

	properties_data_lamp.DATA_PT_lamp.COMPAT_ENGINES.remove(PhotonRenderer.bl_idname)
	properties_data_lamp.DATA_PT_area.COMPAT_ENGINES.remove(PhotonRenderer.bl_idname)

	#properties_material.MATERIAL_PT_preview.COMPAT_ENGINES.remove(PhotonRenderer.bl_idname)

	for panel_type in render_panel_types:
		bpy.utils.unregister_class(panel_type)
