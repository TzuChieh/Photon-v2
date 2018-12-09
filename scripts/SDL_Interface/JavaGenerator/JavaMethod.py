
class JavaMethod:

	UNIT_INDENT = "\t"

	def __init__(self, name=""):
		self.name = name
		self.return_type = ""
		self.access_level = ""
		self.annotation = ""
		self.inputs = []
		self.content_lines = []

	def set_name(self, name):
		self.name = name

	def set_return_type(self, type_name):
		self.return_type = type_name

	def set_access_level(self, level):
		self.access_level = level

	def add_input(self, type_name, name):
		self.inputs.append(type_name + " " + name)

	def add_content_line(self, content):
		self.content_lines.append(JavaMethod.UNIT_INDENT + content + "\n")

	def get_name(self):
		return self.name

	def get_return_type(self):
		return self.return_type

	def get_access_level(self):
		return self.access_level

	def gen_code(self, indent_amount=0):

		code = ""

		if not self.name:
			print("warning: java method has no name")
			return code

		input_list = ", ".join(self.inputs)
		indention = JavaMethod.UNIT_INDENT * indent_amount

		if self.annotation:
			code += "%s%s\n" % (indention, self.annotation)

		code += "%s%s %s %s(%s)\n" % (
			indention,
			self.access_level,
			self.return_type,
			self.name,
			input_list)

		code += indention + "{\n"
		for line in self.content_lines:
			code += "%s%s" % (indention, line)
		code += indention + "}\n"

		return code + "\n"
