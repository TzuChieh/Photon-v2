from SDLInterface import SDLInterface

from InterfaceGenerator import InterfaceGenerator


class RawGenerator(InterfaceGenerator):

	def __init__(self):
		super().__init__()
		self.interfaces = []

	def add_interface(self, sdl_interface: SDLInterface):
		self.interfaces.append(sdl_interface)

	def generate(self, output_directory):

		file = open(output_directory + "raw_interface.txt", "w")
		for interface in self.interfaces:
			file.write(str(interface))
		file.close()

	def name(self):
		return "raw"
