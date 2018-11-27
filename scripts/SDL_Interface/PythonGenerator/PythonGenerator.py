from InterfaceGenerator import InterfaceGenerator
from SDLInterface import SDLInterface
from .PythonClass import PythonClass
from .PythonMethod import PythonMethod
from . import pysdl_header

import copy
import inspect
from string import capwords
import datetime


class PythonGenerator(InterfaceGenerator):

	def __init__(self):
		super().__init__()
		self.interfaces = []

	def add_interface(self, sdl_interface: SDLInterface):
		self.interfaces.append(copy.deepcopy(sdl_interface))

	def generate(self, output_directory):

		if not self.resolve_interface_extension():
			print("warning: cannot resolve interface extension, suggestions: ")
			print("1. check for typo")
			print("2. is the extended target actually exist")
			print("3. may be possible cyclic extensions")
			return

		file = open(output_directory + "pysdl.py", "w+")

		file.write(
			"# ========================================\n"
			"# NOTE: THIS FILE CONTAINS GENERATED CODE \n"
			"#       DO NOT MODIFY                     \n"
			"# ========================================\n")
		file.write("# last generated: %s \n" % datetime.datetime.now())

		file.write(inspect.getsource(pysdl_header))
		file.write("\n\n")
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

		# generating creator code

		if sdl_interface.has_creator():

			clazz = PythonClass(class_base_name + "Creator")
			clazz.set_inherited_class_name("SDLCreatorCommand")

			# overriding get_full_type
			full_type_method = PythonMethod("get_full_type")
			full_type_method.add_content_line("return \"%s\"" % sdl_interface.get_full_type_name())
			clazz.add_method(full_type_method)

			for sdl_input in sdl_interface.creator.inputs:

				method_name = "set_"
				method_name += sdl_input.name.replace("-", "_")
				input_name = sdl_input.name.replace("-", "_")

				if clazz.has_method(method_name):
					continue

				method = PythonMethod(method_name)
				method.add_input(input_name, "SDLData")
				method.add_content_line("self.set_input(\"%s\", %s)" % (sdl_input.name, input_name))

				clazz.add_method(method)

			code += clazz.gen_code()

		# generating executor code

		for sdl_executor in sdl_interface.executors:

			name_norm = capwords(sdl_executor.name, "-").replace("-", "")
			clazz = PythonClass(class_base_name + name_norm)
			clazz.set_inherited_class_name("SDLExecutorCommand")

			# overriding get_full_type
			full_type_method = PythonMethod("get_full_type")
			full_type_method.add_content_line("return \"%s\"" % sdl_interface.get_full_type_name())
			clazz.add_method(full_type_method)

			# overriding get_name
			get_name_method = PythonMethod("get_name")
			get_name_method.add_content_line("return \"%s\"" % sdl_executor.name)
			clazz.add_method(get_name_method)

			for sdl_input in sdl_executor.inputs:

				method_name = "set_"
				method_name += sdl_input.name.replace("-", "_")
				input_name = sdl_input.name.replace("-", "_")

				if clazz.has_method(method_name):
					continue

				method = PythonMethod(method_name)
				method.add_input(input_name, "SDLData")
				method.add_content_line("self.set_input(\"%s\", %s)" % (sdl_input.name, input_name))

				clazz.add_method(method)

			code += clazz.gen_code()

		return code

	@classmethod
	def gen_class_name(cls, sdl_interface: SDLInterface):
		category_norm = capwords(sdl_interface.category_name, "-").replace("-", "")
		type_norm = capwords(sdl_interface.type_name, "-").replace("-", "")
		return type_norm + category_norm

