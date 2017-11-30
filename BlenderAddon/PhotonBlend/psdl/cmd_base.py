from abc import ABC, abstractmethod


class SdlFragment(ABC):
	def __init__(self):
		super(SdlFragment, self).__init__()

	@abstractmethod
	def to_sdl_fragment(self):
		pass


class SdlCommand(ABC):
	def __init__(self):
		super(SdlCommand, self).__init__()
		self._cmdPrefix = ""

	@abstractmethod
	def to_sdl(self):
		pass


class SdlCommandCache:
	def __init__(self):
		self.sdlCommands = []

	def to_sdl(self):
		strings = []
		for command in self.sdlCommands:
			strings.append(command.to_sdl())

		return "".join(strings)

	def add_command(self, command):
		self.sdlCommands.append(command)

	def add_commands(self, commandcache):
		for command in commandcache.sdlCommands:
			self.add_command(command)