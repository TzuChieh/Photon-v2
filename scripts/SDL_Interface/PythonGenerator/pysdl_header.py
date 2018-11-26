from abc import ABC, abstractmethod


class SDLInput(ABC):

	def __init__(self):
		super().__init__()
		self.name = ""

	@abstractmethod
	def get_type(self):
		pass

	@abstractmethod
	def generate_data(self):
		pass

	def set_name(self, name):
		self.name = name
		return self

	def to_clause(self):
		return self.get_type() + " " + self.name + " " + self.generate_data()


class SDLString(SDLInput):

	def __init__(self, string=""):
		super().__init__()
		self.string = string

	def get_type(self):
		return "string"

	def generate_data(self):
		return self.string


class SDLInteger(SDLInput):

	def __init__(self, integer=0):
		super().__init__()
		self.integer = integer

	def get_type(self):
		return "integer"

	def generate_data(self):
		return str(self.integer)


class SDLReal(SDLInput):

	def __init__(self, real=0):
		super().__init__()
		self.real = real

	def get_type(self):
		return "real"

	def generate_data(self):
		return str(self.real)


class SDLVector3(SDLInput):

	def __init__(self, vector=(0, 0, 0)):
		super().__init__()
		self.vector = vector

	def get_type(self):
		return "vector3"

	def generate_data(self):
		return "\"%f %f %f\"" % (self.vector[0], self.vector[1], self.vector[2])


class SDLQuaternion(SDLInput):
	def __init__(self, quaternion=(0, 0, 0, 0)):
		super().__init__()
		self.quaternion = quaternion

	def get_type(self):
		return "quaternion"

	def generate_data(self):
		return "\"%f %f %f %f\"" % (self.quaternion[0], self.quaternion[1], self.quaternion[2], self.quaternion[3])


class SDLRealArray(SDLInput):

	def __init__(self, array=None):
		super().__init__()
		if array is None:
			self.array = []
		else:
			self.array = array

	def get_type(self):
		return "real-array"

	def generate_data(self):
		fragments = ["{"]
		for real in self.array:
			fragments.append("%f " % real)
		fragments.append("}")
		return "".join(fragments)

	def set_array(self, array):
		self.array = array
		return self

	def add(self, real):
		self.array.append(real)
		return self


class SDLVector3Array(SDLInput):

	def __init__(self, array=None):
		super().__init__()
		if array is None:
			self.array = []
		else:
			self.array = array

	def get_type(self):
		return "vector3-array"

	def generate_data(self):
		fragments = ["{"]
		for vector3 in self.array:
			fragments.append("\"%f %f %f\" " % (vector3[0], vector3[1], vector3[2]))
		fragments.append("}")
		return "".join(fragments)

	def set_array(self, array):
		self.array = array
		return self

	def add(self, vector3):
		self.array.append(vector3)
		return self


class SDLReference(SDLInput):
	def __init__(self, ref_type="", ref_name=""):
		super().__init__()
		self.ref_type = ref_type
		self.ref_name = ref_name

	def get_type(self):
		return self.ref_type

	def generate_data(self):
		return "\"@%s\"" % self.ref_name


class SDLRaw(SDLInput):

	def __init__(self, type_string="", data_string=""):
		super().__init__()
		self.type_string = type_string
		self.data_string = data_string

	def get_type(self):
		return self.type_string

	def generate_data(self):
		return self.data_string


class SDLCommand(ABC):

	def __init__(self):
		super().__init__()

	@abstractmethod
	def get_prefix(self):
		pass

	@abstractmethod
	def get_full_type(self):
		pass

	@abstractmethod
	def generate(self):
		pass


class SDLCreatorCommand(SDLCommand):

	def __init__(self):
		super().__init__()
		self.__data_name = ""
		self.__inputs = []

	@abstractmethod
	def get_full_type(self):
		pass

	def get_prefix(self):
		return "->"

	def generate(self):

		# TODO: some part can be pre-generated
		fragments = [
			self.get_prefix(), " ",
			self.get_full_type(), " ",
			"\"@" + self.__data_name + "\"", " "]

		for sdl_input in self.__inputs:
			fragments.append("[")
			fragments.append(sdl_input.to_clause())
			fragments.append("]")

		fragments.append("\n")

		return "".join(fragments)

	def set_data_name(self, data_name):
		self.__data_name = data_name

	def set_input(self, sdl_input: SDLInput):
		self.__inputs.append(sdl_input)
