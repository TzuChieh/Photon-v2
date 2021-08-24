#include "InterfaceGen/Python/PythonGenerator.h"

#include <ph_cpp_core.h>
#include <Utility/Timestamp.h>
#include <DataIO/io_utils.h>

#include <utility>

namespace ph::sdlgen
{

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
		io_utils::load_text(makeResourcePath("pysdl_base.py"));

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

}// end namespace ph::sdlgen
