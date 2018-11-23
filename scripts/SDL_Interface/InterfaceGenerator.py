from SDLInterface import SDLInterface

from abc import abstractmethod


class InterfaceGenerator:

	def __init__(self):
		pass

	@abstractmethod
	def add_interface(self, sdl_interface: SDLInterface):
		pass

	@abstractmethod
	def generate(self, output_directory):
		pass

	@abstractmethod
	def name(self):
		pass
