
class MetaGetter:
	def __init__(self, context):
		self.__context = context

	def render_width_px(self):
		original_width = self.__context.scene.render.resolution_x
		return int(original_width * self.__render_size_fraction())

	def render_height_px(self):
		original_height = self.__context.scene.render.resolution_y
		return int(original_height * self.__render_size_fraction())

	def spp(self):
		return self.__context.scene.ph_render_num_spp

	def sample_filter_name(self):
		filter_type = self.__context.scene.ph_render_sample_filter_type
		if filter_type == "BOX":
			return "box"
		elif filter_type == "GAUSSIAN":
			return "gaussian"
		elif filter_type == "MN":
			return "mn"
		elif filter_type == "BH":
			return "bh"
		else:
			print("warning: unsupported filter type %s, using box BH instead" % filter_type)
			return "bh"

	def integrator_type_name(self):
		integrator_type = self.__context.scene.ph_render_integrator_type
		if integrator_type == "BVPT":
			return "bvpt"
		elif integrator_type == "BNEEPT":
			return "bneept"
		else:
			print("warning: unsupported integrator type %s, using BVPT instead" % integrator_type)
			return "bvpt"

	def __render_size_fraction(self):
		return self.__context.scene.render.resolution_percentage / 100.0
