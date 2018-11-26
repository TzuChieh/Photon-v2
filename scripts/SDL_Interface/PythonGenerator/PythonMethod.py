
class PythonMethod:

	UNIT_INDENT = "    "

	def __init__(self, name=""):
		self.name = name
		self.inputs = ["self"]
		self.content_lines = []

	def set_name(self, name):
		self.name = name

	def add_input(self, name, expected_type=""):
		if not expected_type:
			self.inputs.append(name)
		else:
			self.inputs.append(name + ": " + expected_type)

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
