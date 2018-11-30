
class PythonMethod:

	UNIT_INDENT = "    "

	def __init__(self, name=""):
		self.name = name
		self.inputs = ["self"]
		self.content_lines = []

	def set_name(self, name):
		self.name = name

	# TODO: use keyword arg, otherwise we cannot default to None
	def add_input(self, name, **kwargs):
		input_string = name
		if "expected_type" in kwargs:
			input_string += ": " + kwargs["expected_type"]
		elif "default_value" in kwargs:
			input_string += " = " + kwargs["default_value"]
		self.inputs.append(input_string)

	def add_content_line(self, content):
		self.content_lines.append(PythonMethod.UNIT_INDENT + content + "\n")

	def get_name(self):
		return self.name

	def gen_code(self, indent_amount=0):

		code = ""

		if not self.name:
			print("warning: python method has no name")
			return code

		input_list = ", ".join(self.inputs)
		indention = PythonMethod.UNIT_INDENT * indent_amount

		code += "%sdef %s(%s):\n" % (indention, self.name, input_list)

		if self.content_lines:
			for line in self.content_lines:
				code += "%s%s" % (indention, line)
		else:
			code += indention + PythonMethod.UNIT_INDENT + "pass\n"

		return code + "\n"
