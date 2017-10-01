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
