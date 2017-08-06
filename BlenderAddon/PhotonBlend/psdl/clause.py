from mathutils import Vector
from mathutils import Quaternion

from abc import ABC, abstractmethod


class Clause(ABC):
	def __init__(self):
		self.typeName = ""
		self.name     = ""
		self.data     = ""

	@abstractmethod
	def set_data(self, data):
		pass

	def set_name(self, name):
		self.name = name
		return self

	def to_sdl(self):
		return "[{} {} {}]".format(self.typeName, self.name, self.data)


class IntegerClause(Clause):
	def __init__(self):
		super(IntegerClause, self).__init__()
		self.typeName = "integer"

	def set_data(self, integer):
		self.data = str(integer)
		return self


class FloatClause(Clause):
	def __init__(self):
		super(FloatClause, self).__init__()
		self.typeName = "real"

	def set_data(self, floatvalue):
		self.data = str(floatvalue)
		return self


class StringClause(Clause):
	def __init__(self):
		super(StringClause, self).__init__()
		self.typeName = "string"

	def set_data(self, string):
		self.data = str(string)
		return self


class Vector3Clause(Clause):
	def __init__(self):
		super(Vector3Clause, self).__init__()
		self.typeName = "vector3r"

	def set_data(self, vec3):
		self.data = "\"%.8f %.8f %.8f\"" % (vec3.x, vec3.y, vec3.z)
		return self


class QuaternionClause(Clause):
	def __init__(self):
		super(QuaternionClause, self).__init__()
		self.typeName = "quaternionR"

	def set_data(self, quat):
		self.data = "\"%.8f %.8f %.8f %.8f\"" % (quat.x, quat.y, quat.z, quat.w)
		return self


class Vector3ListClause(Clause):
	def __init__(self):
		super(Vector3ListClause, self).__init__()
		self.typeName = "vector3r-array"

	def set_data(self, vec3arr):
		vec3strings = []
		for vec3 in vec3arr:
			vec3strings.append("\"%.8f %.8f %.8f\"" % (vec3.x, vec3.y, vec3.z))

		self.data = "".join(vec3strings)
		return self
