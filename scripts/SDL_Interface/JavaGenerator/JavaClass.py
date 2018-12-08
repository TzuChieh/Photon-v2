from .JavaMethod import JavaMethod


class JavaClass:

	def __init__(self, name=""):
		self.name = name
		self.inherited_class_name = ""
		self.access_level = "public"
		self.methods = []

	def add_method(self, method: JavaMethod):
		self.methods.append(method)

	def set_inherited_class_name(self, name):
		self.inherited_class_name = name

	def set_access_level(self, level):
		self.access_level = level

	def has_method(self, name):
		for method in self.methods:
			if method.get_name() == name:
				return True
		return False

	def gen_code(self):

		code = ""

		if not self.name:
			print("warning: java class has no name")
			return code

		if self.inherited_class_name:
			code += "%s class %s extends %s\n" % (
				self.access_level,
				self.name,
				self.inherited_class_name)
		else:
			code += "%s class %s\n" % (self.access_level, self.name)

		code += "{\n"
		for method in self.methods:
			code += method.gen_code(1)
		code += "}\n"

		return code + "\n"
