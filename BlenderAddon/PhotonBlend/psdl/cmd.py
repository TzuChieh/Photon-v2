from . import cmd_base

from abc import abstractmethod


class WorldCommand(cmd_base.SdlCommand):

	def __init__(self):
		super(WorldCommand, self).__init__()
		self.__clauses   = {}
		self.__data_name = ""

	@abstractmethod
	def get_type_category(self):
		pass

	@abstractmethod
	def get_type_name(self):
		pass

	@abstractmethod
	def to_sdl(self, sdlconsole):
		pass

	def set_data_name(self, name):
		self.__data_name = name

	def update_clause(self, clause):
		clause_type_and_name = clause.get_type_name() + clause.get_name()
		self.__clauses[clause_type_and_name] = clause

	def get_data_name(self):
		return self.__data_name

	def get_clauses(self):
		return self.__clauses

	def _get_formated_data_name(self):
		return "\"@" + self.__data_name + "\""


class CreationCommand(WorldCommand):

	def __init__(self):
		super(CreationCommand, self).__init__()

	@abstractmethod
	def get_type_category(self):
		pass

	@abstractmethod
	def get_type_name(self):
		pass

	def to_sdl(self, sdlconsole):
		cmdstrs = ["-> ",
		           self.get_type_category(), "(", self.get_type_name(), ") ",
		           self._get_formated_data_name(), " "]
		for clause_name, clause in self.get_clauses().items():
			cmdstrs.append(clause.to_sdl_fragment())
		cmdstrs.append("\n")

		return "".join(cmdstrs)


class FunctionCommand(WorldCommand):

	def __init__(self):
		super(FunctionCommand, self).__init__()
		self.__func_name = ""

	@abstractmethod
	def get_type_category(self):
		pass

	@abstractmethod
	def get_type_name(self):
		pass

	def to_sdl(self, sdlconsole):
		cmdstrs = ["-> ",
		           self.get_type_category(), "(", self.get_type_name(), ") ",
		           self.get_func_name(), "(", self._get_formated_data_name(), ") "]
		for clause_name, clause in self.get_clauses().items():
			cmdstrs.append(clause.to_sdl_fragment())
		cmdstrs.append("\n")

		return "".join(cmdstrs)

	def set_func_name(self, name):
		self.__func_name = name

	def get_func_name(self):
		return self.__func_name


class RawCommand(cmd_base.SdlCommand):

	def __init__(self):
		super(RawCommand, self).__init__()
		self.__command_string = ""

	def append_string(self, string):
		self.__command_string += string

	def to_sdl(self, sdlconsole):
		return self.__command_string


# TODO: core commands
