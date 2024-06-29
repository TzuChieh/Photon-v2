from utility import settings, blender

import bpy


@blender.register_class
class PH_WORLD_PT_background(bpy.types.Panel):
	bl_label = "PR: Background"
	bl_context = 'world'
	bl_space_type = 'PROPERTIES'
	bl_region_type = 'WINDOW'

	@classmethod
	def poll(cls, b_context):
		render_settings = b_context.scene.render
		return render_settings.engine in settings.photon_engines and b_context.world

	def draw(self, b_context):
		b_layout = self.layout
		b_world = b_context.world

		b_layout.prop(b_world.photon, 'background_type')
		
		bg_type = b_world.photon.background_type
		if bg_type != 'NONE':
			b_layout.prop(b_world.photon, 'up_rotation')
		
		if bg_type == 'IMAGE':
			b_layout.prop(b_world.photon, 'image_file_path')
		elif bg_type == 'PREETHAM':
			b_layout.prop(b_world.photon, 'preetham_turbidity')
			b_layout.prop(b_world.photon, 'standard_time')
			b_layout.prop(b_world.photon, 'standard_meridian')
			b_layout.prop(b_world.photon, 'latitude')
			b_layout.prop(b_world.photon, 'longitude')
			b_layout.prop(b_world.photon, 'julian_date')

		if bg_type != 'NONE':
			b_layout.prop(b_world.photon, 'energy_scale')
