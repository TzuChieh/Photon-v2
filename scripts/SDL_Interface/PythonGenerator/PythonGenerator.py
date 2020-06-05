from InterfaceGenerator import InterfaceGenerator
from SDLInterface import SDLInterface
from .PythonClass import PythonClass
from .PythonMethod import PythonMethod
from . import pysdl_base
import sdl_types

import inspect
from string import capwords
import datetime


class PythonGenerator(InterfaceGenerator):
	def generate(self, output_directory):
		file = open(output_directory + "pysdl.py", "w+")

		file.write(
			"# ========================================\n"
			"# NOTE: THIS FILE CONTAINS GENERATED CODE \n"
			"#       DO NOT MODIFY                     \n"
			"# ========================================\n")
		file.write("# last generated: %s \n\n" % datetime.datetime.now())

		file.write(inspect.getsource(pysdl_base))

		file.write("\n\n")
		file.write(PythonGenerator.gen_reference_data_classes())

		file.write("\n\n")
		for interface in self.interfaces:
			file.write(PythonGenerator.gen_interface_classes(interface))
		file.close()

	def name(self):
		return "python"

	@classmethod
	def gen_reference_data_classes(cls):
		code = ""
		for type_name in sdl_types.REFERENCE_TYPES:
			class_name = capwords(type_name, "-").replace("-", "")

			clazz = PythonClass("SDL" + class_name)
			clazz.set_inherited_class_name("SDLReference")

			init_method = PythonMethod("__init__")
			init_method.add_input("ref_name", default_value="\"\"")
			init_method.add_content_line("super().__init__(\"%s\", ref_name)" % type_name)
			clazz.add_method(init_method)

			code += clazz.gen_code()

		return code

	@classmethod
	def gen_interface_classes(cls, sdl_interface: SDLInterface):
		class_base_name = cls.gen_class_name(sdl_interface)

		code = ""

		# Generate creator code

		if sdl_interface.has_creator() and not sdl_interface.creator.is_blueprint:
			clazz = PythonClass(class_base_name + "Creator")
			clazz.set_inherited_class_name("SDLCreatorCommand")
			clazz.add_default_init()

			# Override get_full_type()
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
				method.add_input(input_name, expected_type="SDLData")
				method.add_content_line("self.set_input(\"%s\", %s)" % (sdl_input.name, input_name))

				clazz.add_method(method)

			code += clazz.gen_code()

		# Generate executor code

		for sdl_executor in sdl_interface.executors:
			name_norm = capwords(sdl_executor.name, "-").replace("-", "")
			clazz = PythonClass(class_base_name + name_norm)
			clazz.set_inherited_class_name("SDLExecutorCommand")

			# Override get_full_type()
			full_type_method = PythonMethod("get_full_type")
			full_type_method.add_content_line("return \"%s\"" % sdl_interface.get_full_type_name())
			clazz.add_method(full_type_method)

			# Override get_name()
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
				method.add_input(input_name, expected_type="SDLData")
				method.add_content_line("self.set_input(\"%s\", %s)" % (sdl_input.name, input_name))

				clazz.add_method(method)

			code += clazz.gen_code()

		return code

	@classmethod
	def gen_class_name(cls, sdl_interface: SDLInterface):
		category_norm = capwords(sdl_interface.category_name, "-").replace("-", "")
		type_norm = capwords(sdl_interface.type_name, "-").replace("-", "")
		return type_norm + category_norm
