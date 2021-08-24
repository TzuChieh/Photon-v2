#include "InterfaceGen/Python/PythonGenerator.h"
#include "InterfaceGen/Python/PythonClass.h"
#include "InterfaceGen/Python/PythonMethod.h"
#include "ph_sdlgen.h"

#include <ph_cpp_core.h>
#include <Utility/Timestamp.h>
#include <DataIO/io_utils.h>

#include <utility>

namespace ph::sdlgen
{

namespace
{

PythonClass gen_sdl_reference_class(const std::string_view categoryName);
PythonClass gen_sdl_interface_class(const SdlClass* sdlClass);

}

PythonGenerator::PythonGenerator(Path outputDirectory) :

	InterfaceGenerator("python", std::move(outputDirectory)),

	m_file()
{}

void PythonGenerator::generate(
	const std::vector<const SdlClass*>& sdlClasses,
	const std::vector<const SdlEnum*>&  sdlEnums)
{
	m_file = FormattedTextFileOutputStream(makeOutputFilePath("pysdl.py"));

	m_file.writeString(
		"# ========================================\n"
		"# NOTE: THIS FILE CONTAINS GENERATED CODE \n"
		"#       DO NOT MODIFY                     \n"
		"# ========================================\n");
	m_file.write("# last generated: {} \n\n", Timestamp().toString());

	m_file.writeString(
		io_utils::load_text(makeResourcePath("pysdl_base.py")));

	m_file.write("\n\n");

	file.write(PythonGenerator.gen_reference_data_classes())

	m_file.write("\n\n");

	for interface in self.interfaces:
		file.write(PythonGenerator.gen_interface_classes(interface))
}

Path PythonGenerator::makeResourcePath(const std::string& fileSubPath) const
{
	return Path(get_internal_resource_directory(EPhotonProject::SDL_GEN))
		.append(Path("PythonGenerator"))
		.append(Path(fileSubPath));
}

namespace
{

PythonClass gen_sdl_reference_class(const std::string_view categoryName)
{
	PythonClass clazz("SDL" + sdl_name_to_camel_case(categoryName, true));
	clazz.setInheritedClass("SDLReference");

	PythonMethod initMethod("__init__");
	initMethod.addInput("ref_name", "\"\"");
	initMethod.addCodeLine("super().__init__(\"{}\", ref_name)", categoryName);
	clazz.addMethod(initMethod);

	return clazz;
}

PythonClass gen_sdl_interface_class(const SdlClass* const sdlClass)
{
	PH_ASSERT(sdlClass);

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
}

}

}// end namespace ph::sdlgen
