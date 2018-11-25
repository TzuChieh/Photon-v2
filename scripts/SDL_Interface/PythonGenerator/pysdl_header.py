from abc import ABC, abstractmethod


class SDLCommand(ABC):

	def __init__(self):
		super().__init__()

	@abstractmethod
	def get_prefix(self):
		pass

	@abstractmethod
	def get_full_type_name(self):
		pass

	@abstractmethod
	def to_sdl(self):
		pass


class SDLCreatorCommand(SDLCommand):

	def __init__(self):
		super().__init__()
		self.__data_name = ""
		self.__inputs = []

	def get_prefix(self):
		return "->"

	@abstractmethod
	def get_full_type_name(self):
		pass

	def to_sdl(self):

		# TODO: some part can be pre-generated
		fragments = [
			self.get_prefix(), " ",
			self.get_full_type_name(), " ",
			"\"@" + self.__data_name + "\"", " "]

		for clause_content in self.__inputs:
			fragments.append("[")
			fragments.extend(clause_content)
			fragments.append("]")

		fragments.append("\n")

		return "".join(fragments)

	def set_data_name(self, data_name):
		self.__data_name = data_name

	def set_value_input(self, type_name, name, data):
		self.__inputs.append((type_name, name, data))

	def set_reference_input(self, type_name, name, data):
		self.__inputs.append((type_name, "\"@" + name + "\"", data))
