from abc import ABC, abstractmethod


class SDLData(ABC):

	def __init__(self):
		super().__init__()

	@abstractmethod
	def get_type(self):
		pass

	@abstractmethod
	def generate_data(self):
		pass


class SDLString(SDLData):

	def __init__(self, string=""):
		super().__init__()
		self.string = string

	def get_type(self):
		return "string"

	def generate_data(self):
		return self.string


class SDLInteger(SDLData):

	def __init__(self, integer=0):
		super().__init__()
		self.integer = integer

	def get_type(self):
		return "integer"

	def generate_data(self):
		return str(self.integer)


class SDLReal(SDLData):

	def __init__(self, real=0):
		super().__init__()
		self.real = real

	def get_type(self):
		return "real"

	def generate_data(self):
		return str(self.real)


class SDLVector3(SDLData):

	def __init__(self, vector=(0, 0, 0)):
		super().__init__()
		self.vector = vector

	def get_type(self):
		return "vector3"

	def generate_data(self):
		return "\"%f %f %f\"" % (self.vector[0], self.vector[1], self.vector[2])


class SDLQuaternion(SDLData):
	def __init__(self, quaternion=(0, 0, 0, 0)):
		super().__init__()
		self.quaternion = quaternion

	def get_type(self):
		return "quaternion"

	def generate_data(self):
		return "\"%f %f %f %f\"" % (self.quaternion[0], self.quaternion[1], self.quaternion[2], self.quaternion[3])


class SDLRealArray(SDLData):

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


class SDLVector3Array(SDLData):

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


# TODO: make a reference type for each category
class SDLReference(SDLData):
	def __init__(self, ref_type="", ref_name=""):
		super().__init__()
		self.ref_type = ref_type
		self.ref_name = ref_name

	def get_type(self):
		return self.ref_type

	def generate_data(self):
		return "\"@%s\"" % self.ref_name


class SDLRaw(SDLData):

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
		self._inputs = []

	@abstractmethod
	def get_prefix(self):
		pass

	@abstractmethod
	def get_full_type(self):
		pass

	@abstractmethod
	def generate(self):
		pass

	def set_input(self, name, data: SDLData):
		self._inputs.append((name, data))

	def _generate_input_fragments(self, out_fragments):
		for name, data in self._inputs:
			out_fragments.append("[")
			out_fragments.append(data.get_type() + " ")
			out_fragments.append(name + " ")
			out_fragments.append(data.generate_data())
			out_fragments.append("]")


class SDLCreatorCommand(SDLCommand):

	def __init__(self):
		super().__init__()
		self.__data_name = ""

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
		self._generate_input_fragments(fragments)
		fragments.append("\n")

		return "".join(fragments)

	def set_data_name(self, data_name):
		self.__data_name = data_name


class SDLExecutorCommand(SDLCommand):

	def __init__(self):
		super().__init__()
		self.__target_name = ""
		self.__inputs = []

	@abstractmethod
	def get_full_type(self):
		pass

	@abstractmethod
	def get_name(self):
		pass

	def get_prefix(self):
		return "->"

	def generate(self):

		# TODO: some part can be pre-generated
		fragments = [
			self.get_prefix(), " ",
			self.get_full_type(), " ",
			self.get_name(), "(",
			"\"@" + self.__target_name + "\")", " "]
		self._generate_input_fragments(fragments)
		fragments.append("\n")

		return "".join(fragments)

	def set_target_name(self, data_name):
		self.__target_name = data_name


class SDLCoreCommand(SDLCommand):

	def __init__(self):
		super().__init__()

	@abstractmethod
	def get_full_type(self):
		pass

	def get_prefix(self):
		return "##"

	def generate(self):

		fragments = [
			self.get_prefix(), " ",
			self.get_full_type(), " "]
		self._generate_input_fragments(fragments)
		fragments.append("\n")

		return "".join(fragments)
