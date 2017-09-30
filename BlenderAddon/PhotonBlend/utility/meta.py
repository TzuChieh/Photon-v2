
class MetaGetter:
	def __init__(self, context):
		self.__context = context

	def render_width_px(self):
		original_width = self.__context.scene.render.resolution_x
		return original_width * self.__render_size_fraction()

	def render_height_px(self):
		original_height = self.__context.scene.render.resolution_y
		return original_height * self.__render_size_fraction()

	def __render_size_fraction(self):
		return self.__context.scene.render.resolution_percentage / 100.0
