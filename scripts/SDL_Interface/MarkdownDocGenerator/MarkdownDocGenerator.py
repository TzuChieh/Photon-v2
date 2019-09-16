from InterfaceGenerator import InterfaceGenerator
from SDLInterface import SDLInterface
from SDLCreator import SDLCreator
from SDLExecutor import SDLExecutor
from SDLInput import SDLInput
from SDLStruct import SDLStruct
import sdl_types

import inspect
from string import capwords
import datetime

# TODO: cross referencing
# TODO: value types


class MarkdownDocGenerator(InterfaceGenerator):
	def __init__(self):
		super().__init__()
		self.__raw_string = ""

	def generate(self, output_directory):
		self.write_line("# Photon Scene Description Language")
		self.write_empty_line()

		for interface in self.interfaces:
			self.write_interface(interface)

		for struct in self.structs:
			self.write_struct(struct)

		file = open(output_directory + "markdown_doc.md", "w+")
		file.write(self.__raw_string)
		file.close()

		self.__raw_string = ""

	def name(self):
		return "markdown-doc"

	def write_interface(self, sdl_interface: SDLInterface):
		self.write_line("## " + sdl_interface.name)
		self.write_empty_line()
		self.write_line("* Category: `%s`" % sdl_interface.category_name)
		self.write_line("* Type: `%s`" % sdl_interface.type_name)
		self.write_empty_line()
		self.write_line(sdl_interface.description)
		self.write_empty_line()

		if sdl_interface.has_creator():
			self.write_creator(sdl_interface.creator)

		if sdl_interface.has_executor():
			for sdl_executor in sdl_interface.executors:
				self.write_executor(sdl_executor)

	def write_creator(self, sdl_creator: SDLCreator):
		if sdl_creator.is_blueprint:
			return

		self.write_line("> Creation:")
		self.write_empty_line()
		self.write_inputs(sdl_creator.inputs)

	def write_executor(self, sdl_executor: SDLExecutor):
		self.write_line("> Operation **%s**:" % sdl_executor.name)
		self.write_empty_line()
		self.write_line(sdl_executor.description)
		self.write_empty_line()
		self.write_inputs(sdl_executor.inputs)

	def write_inputs(self, sdl_inputs):
		if not sdl_inputs:
			self.write_line("(no input)")
			self.write_empty_line()
			return

		self.write_line("| Inputs | Types | Descriptions |")
		self.write_line("| --- | --- | --- |")
		for sdl_input in sdl_inputs:
			self.write_line("| %s | `%s` | %s |" % (sdl_input.name, sdl_input.type_name, sdl_input.description))
		self.write_empty_line()

	def write_struct(self, sdl_struct: SDLStruct):
		self.write_line("## Data Structure: `%s`" % sdl_struct.type_name)
		self.write_empty_line()
		self.write_inputs(sdl_struct.inputs)

	def write_line(self, line):
		self.__raw_string += line
		self.__raw_string += "\n"

	def write_empty_line(self):
		self.__raw_string += "\n"
