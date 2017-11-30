from . import cmd_base

from mathutils import Vector
from mathutils import Quaternion

from abc import abstractmethod


class SdlClause(cmd_base.SdlFragment):
	def __init__(self):
		super(SdlClause, self).__init__()
		self.typeName = ""
		self.name     = ""
		self.data     = ""

	@abstractmethod
	def set_data(self, data):
		pass

	def to_sdl_fragment(self):
		return "[{} {} {}]".format(self.typeName, self.name, self.data)

	def set_name(self, name):
		self.name = name
		return self


class IntegerClause(SdlClause):
	def __init__(self):
		super(IntegerClause, self).__init__()
		self.typeName = "integer"

	def set_data(self, integer):
		self.data = str(integer)
		return self


class FloatClause(SdlClause):
	def __init__(self):
		super(FloatClause, self).__init__()
		self.typeName = "real"

	def set_data(self, floatvalue):
		self.data = str(floatvalue)
		return self


class StringClause(SdlClause):
	def __init__(self):
		super(StringClause, self).__init__()
		self.typeName = "string"

	def set_data(self, string):
		self.data = str(string)
		return self


class Vector3Clause(SdlClause):
	def __init__(self):
		super(Vector3Clause, self).__init__()
		self.typeName = "vector3r"

	def set_data(self, vec3):
		self.data = "\"%.8f %.8f %.8f\"" % (vec3.x, vec3.y, vec3.z)
		return self


class ColorClause(SdlClause):
	def __init__(self):
		super(ColorClause, self).__init__()
		self.typeName = "vector3r"

	def set_data(self, color):
		self.data = "\"%.8f %.8f %.8f\"" % (color.r, color.g, color.b)
		return self


class QuaternionClause(SdlClause):
	def __init__(self):
		super(QuaternionClause, self).__init__()
		self.typeName = "quaternionR"

	def set_data(self, quat):
		self.data = "\"%.8f %.8f %.8f %.8f\"" % (quat.x, quat.y, quat.z, quat.w)
		return self


class Vector3ListClause(SdlClause):
	def __init__(self):
		super(Vector3ListClause, self).__init__()
		self.typeName = "vector3r-array"

	def set_data(self, vec3arr):
		vec3strings = ['{']
		for vec3 in vec3arr:
			vec3strings.append("\"%.8f %.8f %.8f\"" % (vec3.x, vec3.y, vec3.z))
		vec3strings.append('}')

		self.data = "".join(vec3strings)
		return self
