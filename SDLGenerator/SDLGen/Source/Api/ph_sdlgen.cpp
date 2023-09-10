#include "ph_sdlgen.h"
#include "InterfaceGen/MarkdownDoc/MarkdownDocGenerator.h"

#include <Common/logging.h>
#include <Utility/utility.h>
#include <DataIO/FileSystem/Path.h>
#include <DataIO/FileSystem/Filesystem.h>
#include <ph_cpp_core.h>

#include <string>

namespace ph::sdlgen
{

PH_DEFINE_INTERNAL_LOG_GROUP(SdlGenApi, SDLGen);

void generate_sdl_interface(const EInterfaceGenerator type, const std::string_view outputDirectoryStr)
{
	auto outputDirectory = Path(std::string(outputDirectoryStr));
	Filesystem::createDirectories(outputDirectory);

	if(!Filesystem::hasDirectory(outputDirectory))
	{
		PH_LOG_WARNING(SdlGenApi, "cannot create output directory <{}>",
			outputDirectory.toAbsoluteString());
	}

	auto generator = InterfaceGenerator::makeGenerator(type, outputDirectory);
	if(!generator)
	{
		PH_LOG_WARNING(SdlGenApi, "invalid generator type <{}> provided, nothing is generated",
			enum_to_string(type));
		return;
	}

	PH_ASSERT(generator);

	PH_LOG(SdlGenApi, "generating SDL interface via generator {}", generator->getName());

	generator->generate(
		get_registered_engine_classes(),
		get_registered_engine_enums());

	PH_LOG(SdlGenApi, "done generating SDL interface, output to <{}>",
		generator->getOutputDirectory().toAbsoluteString());
}

}// end namespace ph::sdlgen
