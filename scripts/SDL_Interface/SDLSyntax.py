class SDLSyntax:

	core_command_prefix = "##"
	world_command_prefix = "->"
	comment_prefix = "//"
	input_packet_begin = "["
	input_packet_end = "]"

	@classmethod
	def gen_full_type_name(cls, category_name, type_name):
		return category_name + "(" + type_name + ")"

	@classmethod
	def gen_formatted_data_name(cls, data_name):
		return "\"@" + data_name + "\""
