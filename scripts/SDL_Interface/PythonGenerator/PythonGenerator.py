from InterfaceGenerator import InterfaceGenerator
from SDLInterface import SDLInterface
from .PythonClass import PythonClass
from .PythonMethod import PythonMethod
from . import pysdl_header

import copy
import inspect


class PythonGenerator(InterfaceGenerator):

	TYPE_ABBR = {

	}

	def __init__(self):
		super().__init__()
		self.interfaces = []

	def add_interface(self, sdl_interface: SDLInterface):
		self.interfaces.append(copy.deepcopy(sdl_interface))

	def generate(self, output_directory):

		if not self.resolve_interface_extension():
			print("warning: cannot resolve interface extension, may be possible cyclic extensions")
			return

		file = open(output_directory + "pysdl.py", "w+")
		file.write(inspect.getsource(pysdl_header))
		for interface in self.interfaces:
			file.write(PythonGenerator.gen_interface_classes(interface))
		file.close()

	def name(self):
		return "python"

	def resolve_interface_extension(self):

		resolved_interfaces = {}
		unresolved_interfaces = []
		for interface in self.interfaces:
			if interface.is_extending():
				unresolved_interfaces.append(interface)
			else:
				resolved_interfaces[interface.get_full_type_name()] = interface

		while unresolved_interfaces:

			has_progress = False

			for interface in unresolved_interfaces:
				target_name = interface.get_extended_full_type_name()
				extended_interface = resolved_interfaces.get(target_name, None)
				if extended_interface is None:
					continue
				else:
					interface.extend(extended_interface)
					resolved_interfaces[interface.get_full_type_name()] = interface
					unresolved_interfaces.remove(interface)
					has_progress = True

			if not has_progress:
				return False

		return True

	@classmethod
	def gen_interface_classes(cls, sdl_interface: SDLInterface):

		class_base_name = cls.gen_class_name(sdl_interface)

		code = ""

		if sdl_interface.has_creator():

			clazz = PythonClass(class_base_name + "Creator")
			clazz.set_inherited_class_name("SDLCreatorCommand")

			for sdl_input in sdl_interface.creator.inputs:

				method_name = "set_"
				method_name += sdl_input.type_name.replace("-", "_") + "_"
				method_name += sdl_input.name.replace("-", "_")
				method = PythonMethod(method_name)
				method.add_input(sdl_input.name)

				if sdl_input.is_value():
					method.add_content_line("self.set_value_input(\"%s\", \"%s\", %s)" % (
						sdl_input.type_name, sdl_input.name, "str(%s)" % sdl_input.name))
				else:
					method.add_content_line("self.set_reference_input(\"%s\", \"%s\", %s)" % (
						sdl_input.type_name, sdl_input.name, sdl_input.name))

				clazz.add_method(method)

			code += clazz.gen_code()

		return code

	@classmethod
	def gen_class_name(cls, sdl_interface: SDLInterface):
		category_norm = sdl_interface.category_name.capwords("-")
		type_norm = sdl_interface.type_name.capwords("-")
		return type_norm + category_norm

