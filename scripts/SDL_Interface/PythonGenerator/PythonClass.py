from .PythonMethod import PythonMethod


class PythonClass:

	def __init__(self, name=""):
		self.name = name
		self.inherited_class_name = ""
		self.methods = []

	def add_method(self, method: PythonMethod):
		self.methods.append(method)

	def set_inherited_class_name(self, name):
		self.inherited_class_name = name

	def gen_code(self):

		code = ""

		if not self.name:
			print("warning: python class has no name")
			return code

		if self.inherited_class_name:
			code += "class %s(%s):\n" % (self.name, self.inherited_class_name)
			code += "    def __init__(self):\n"
			code += "        super().__init__()\n"
		else:
			code += "class %s:\n" % self.name

		if self.methods:
			for method in self.methods:
				code += method.gen_code(1)
		else:
			code += "    pass\n"

		return code