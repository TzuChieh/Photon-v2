from . import SdlCommand

from abc import abstractmethod


class WorldCommand(SdlCommand):
	def __init__(self):
		super(WorldCommand, self).__init__()
		self._typeCategory = ""
		self._typeName     = ""
		self._dataName     = ""
		self._clauses      = []

		self._cmdPrefix = "->"

	@abstractmethod
	def to_sdl(self):
		pass

	def set_type_info(self, categoryname, typename):
		self._typeCategory = categoryname
		self._typeName     = typename

	def set_data_name(self, name):
		self._dataName = name

	def add_clause(self, clause):
		self._clauses.append(clause)


class CreationCommand(WorldCommand):
	def __init__(self):
		super(CreationCommand, self).__init__()

	def to_sdl(self):
		cmdstrs = [self._cmdPrefix, " ",
		           self._typeCategory, "(", self._typeName, ") ",
		           self._dataName, " "]
		for clause in self._clauses:
			cmdstrs.append(clause.to_sdl_fragment())
		cmdstrs.append("\n")

		return "".join(cmdstrs)


class FunctionCommand(WorldCommand):
	def __init__(self):
		super(FunctionCommand, self).__init__()
		self._funcName = ""

	def to_sdl(self):
		cmdstrs = [self._cmdPrefix, " ",
		           self._typeCategory, "(", self._typeName, ") ",
		           self._funcName, "(", self._dataName, ") "]
		for clause in self._clauses:
			cmdstrs.append(clause.to_sdl_fragment())
		cmdstrs.append("\n")

		return "".join(cmdstrs)

	def set_func_name(self, name):
		self._funcName = name

