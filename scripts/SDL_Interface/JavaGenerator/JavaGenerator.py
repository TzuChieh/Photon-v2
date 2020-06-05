from InterfaceGenerator import InterfaceGenerator
from SDLInterface import SDLInterface
from SDLInput import SDLInput
from .JavaClass import JavaClass
from .JavaMethod import JavaMethod
import sdl_types

from string import capwords
import datetime
import pathlib
import os
import shutil


class JavaGenerator(InterfaceGenerator):
	def generate(self, output_directory):
		# create package folder

		package_name = "jsdl"
		package_directory = pathlib.Path(output_directory + package_name + "/")
		package_directory.mkdir(parents=True, exist_ok=True)

		# Copy base source files to package folder

		this_folder = os.path.dirname(os.path.abspath(__file__))
		jsdl_base_folder = os.path.join(this_folder, "jsdl_base/")
		for filename in os.listdir(jsdl_base_folder):
			full_filename = os.path.join(jsdl_base_folder, filename)
			shutil.copy(str(full_filename), str(package_directory))

		# Gather generated classes

		clazzes = []

		for type_name in sdl_types.REFERENCE_TYPES:
			clazz = JavaGenerator.gen_reference_data_class(type_name)
			clazzes.append(clazz)

		for interface in self.interfaces:
			clazzes.extend(JavaGenerator.gen_interface_classes(interface))

		# Export classes as java source files

		for clazz in clazzes:
			clazz.package = package_name
			clazz.access_level = "public"

			file = open(os.path.join(str(package_directory), clazz.name + ".java"), "w+")
			file.write(
				"// ========================================\n"
				"// NOTE: THIS FILE CONTAINS GENERATED CODE \n"
				"//       DO NOT MODIFY                     \n"
				"// ========================================\n")
			file.write("// last generated: %s \n\n" % datetime.datetime.now())
			file.write(clazz.gen_code())
			file.close()

	def name(self):
		return "java"

	@classmethod
	def gen_reference_data_class(cls, reference_type_name):
		class_name = "SDL"
		class_name += capwords(reference_type_name, "-").replace("-", "")

		clazz = JavaClass(class_name)
		clazz.set_inherited_class_name("SDLReference")

		constructor = JavaMethod(class_name)
		constructor.access_level = "public"
		constructor.add_input("String", "name")
		constructor.add_content_line("super(\"%s\", name);" % reference_type_name)
		clazz.add_method(constructor)

		return clazz

	@classmethod
	def gen_interface_classes(cls, sdl_interface: SDLInterface):
		clazzes = []
		class_base_name = cls.gen_class_name(sdl_interface)

		# generating creator code

		if sdl_interface.has_creator() and not sdl_interface.creator.is_blueprint:
			class_name = class_base_name + "Creator"
			clazz = JavaClass(class_name)
			clazz.set_inherited_class_name("SDLCreatorCommand")

			# Override getFullType()
			full_type_method = JavaMethod("getFullType")
			full_type_method.annotation = "@Override"
			full_type_method.return_type = "String"
			full_type_method.access_level = "public"
			full_type_method.add_content_line("return \"%s\";" % sdl_interface.get_full_type_name())
			clazz.add_method(full_type_method)

			for sdl_input in sdl_interface.creator.inputs:
				method_name = "set"
				method_name += capwords(sdl_input.name, "-").replace("-", "")

				method = JavaMethod(method_name)
				method.add_input(
					JavaGenerator.get_data_class_name(sdl_input),
					"data")
				method.return_type = "void"
				method.access_level = "public"
				method.add_content_line("setInput(\"%s\", %s);" % (sdl_input.name, "data"))

				clazz.add_method(method)

			clazzes.append(clazz)

		# Generate executor code

		for sdl_executor in sdl_interface.executors:
			name_norm = capwords(sdl_executor.name, "-").replace("-", "")
			class_name = class_base_name + name_norm
			clazz = JavaClass(class_name)
			clazz.set_inherited_class_name("SDLExecutorCommand")

			# Override getFullType()
			full_type_method = JavaMethod("getFullType")
			full_type_method.annotation = "@Override"
			full_type_method.return_type = "String"
			full_type_method.access_level = "public"
			full_type_method.add_content_line("return \"%s\";" % sdl_interface.get_full_type_name())
			clazz.add_method(full_type_method)

			# Override getName()
			get_name_method = JavaMethod("getName")
			get_name_method.annotation = "@Override"
			get_name_method.return_type = "String"
			get_name_method.access_level = "public"
			get_name_method.add_content_line("return \"%s\";" % sdl_executor.name)
			clazz.add_method(get_name_method)

			for sdl_input in sdl_executor.inputs:
				method_name = "set"
				method_name += capwords(sdl_input.name, "-").replace("-", "")

				method = JavaMethod(method_name)
				method.add_input(
					JavaGenerator.get_data_class_name(sdl_input),
					"data")
				method.return_type = "void"
				method.access_level = "public"
				method.add_content_line("setInput(\"%s\", %s);" % (sdl_input.name, "data"))

				clazz.add_method(method)

			clazzes.append(clazz)

		return clazzes

	@classmethod
	def gen_class_name(cls, sdl_interface: SDLInterface):
		category_norm = capwords(sdl_interface.category_name, "-").replace("-", "")
		type_norm = capwords(sdl_interface.type_name, "-").replace("-", "")
		return type_norm + category_norm

	@classmethod
	def get_data_class_name(cls, sdl_input: SDLInput):
		class_name = "SDL"
		class_name += capwords(sdl_input.type_name, "-").replace("-", "")
		return class_name
