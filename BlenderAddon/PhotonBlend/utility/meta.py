
class MetaGetter:
	def __init__(self, b_context):
		self.__context = b_context

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

	def render_method(self):
		return self.__context.scene.ph_render_integrator_type

	def integrator_type_name(self):
		integrator_type = self.__context.scene.ph_render_integrator_type
		if integrator_type == "BVPT":
			return "bvpt"
		elif integrator_type == "BNEEPT":
			return "bneept"
		elif integrator_type == "BVPTDL":
			return "bvptdl"
		else:
			print("warning: unsupported integrator type %s, using BVPT instead" % integrator_type)
			return "bvpt"
