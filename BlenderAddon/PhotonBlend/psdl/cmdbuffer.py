
class SdlCommandBuffer:

	def __init__(self):
		self.__commands = []

	def to_sdl(self):
		strings = []
		for command in self.__commands:
			strings.append(command.to_sdl())

		return "".join(strings)

	def add_command(self, command):
		self.__commands.append(command)

	def add_commands(self, command_buffer):
		for command in command_buffer.__commands:
			self.add_command(command)
