from . import cmd_base

from mathutils import Vector
from mathutils import Quaternion

from abc import abstractmethod


class SdlClause(cmd_base.SdlFragment):

	def __init__(self):
		super(SdlClause, self).__init__()
		self.__name = ""
		self.__data = ""

	@abstractmethod
	def get_type_name(self):
		pass

	@abstractmethod
	def data_to_string(self, data):
		pass

	def to_sdl_fragment(self):
		data_string = self.data_to_string(self.__data)
		return "[{} {} {}]".format(self.get_type_name(), self.__name, data_string)

	def set_name(self, name):
		self.__name = name
		return self

	def set_data(self, data):
		self.__data = data
		return self

	def get_name(self):
		return self.__name


class IntegerClause(SdlClause):

	def __init__(self):
		super(IntegerClause, self).__init__()

	def get_type_name(self):
		return "integer"

	def data_to_string(self, integer_value):
		return str(integer_value)


class FloatClause(SdlClause):

	def __init__(self):
		super(FloatClause, self).__init__()

	def get_type_name(self):
		return "real"

	def data_to_string(self, float_value):
		return str(float_value)


class StringClause(SdlClause):

	def __init__(self):
		super(StringClause, self).__init__()

	def get_type_name(self):
		return "string"

	def data_to_string(self, string):
		return "\"" + string + "\""


class SdlResourceIdentifierClause(StringClause):

	def __init__(self):
		super(SdlResourceIdentifierClause, self).__init__()

	def data_to_string(self, sdlri):
		return super(SdlResourceIdentifierClause, self).data_to_string(sdlri.get_identifier())


class SdlReferenceClause(SdlClause):

	def __init__(self):
		super(SdlReferenceClause, self).__init__()
		self.__reference_type_name = None

	def set_reference_category(self, refence_type_name):
		self.__reference_type_name = refence_type_name
		return self

	def get_type_name(self):
		return self.__reference_type_name

	def data_to_string(self, reference_name):
		return "@\"" + reference_name + "\""


class Vector3Clause(SdlClause):

	def __init__(self):
		super(Vector3Clause, self).__init__()

	def get_type_name(self):
		return "vector3"

	def data_to_string(self, vec3):
		return "\"%.8f %.8f %.8f\"" % (vec3.x, vec3.y, vec3.z)


class ColorClause(Vector3Clause):

	def __init__(self):
		super(ColorClause, self).__init__()

	def data_to_string(self, color):
		return "\"%.8f %.8f %.8f\"" % (color.r, color.g, color.b)


class QuaternionClause(SdlClause):

	def __init__(self):
		super(QuaternionClause, self).__init__()

	def get_type_name(self):
		return "quaternionR"

	def data_to_string(self, quat):
		return "\"%.8f %.8f %.8f %.8f\"" % (quat.x, quat.y, quat.z, quat.w)


class Vector3ListClause(SdlClause):

	def __init__(self):
		super(Vector3ListClause, self).__init__()

	def get_type_name(self):
		return "vector3r-array"

	def data_to_string(self, vec3arr):
		vec3strings = ['{']
		for vec3 in vec3arr:
			vec3strings.append("\"%.8f %.8f %.8f\"" % (vec3.x, vec3.y, vec3.z))
		vec3strings.append('}')
		return "".join(vec3strings)
