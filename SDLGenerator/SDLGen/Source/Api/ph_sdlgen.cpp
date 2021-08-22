#include "ph_sdlgen.h"
#include "InterfaceGen/MarkdownDoc/MarkdownDocGenerator.h"

#include <Utility/string_utils.h>
#include <Common/logging.h>
#include <Utility/utility.h>
#include <DataIO/FileSystem/Path.h>
#include <ph_cpp_core.h>

#include <cstddef>
#include <utility>
#include <string>

namespace ph::sdlgen
{

PH_DEFINE_INTERNAL_LOG_GROUP(SdlGenApi, SDLGen);

std::string sdl_name_to_capitalized(const std::string_view sdlName)
{
	// The implementation is aware of empty inputs

	auto result = std::string(sdlName);

	// Capitalize first character if possible
	if(!result.empty() && result.front() != '-')
	{
		result.front() = string_utils::az_to_AZ(result.front());
	}

	for(std::size_t i = 1; i < result.size(); ++i)
	{
		const char ch = result[i];

		// Find all dashes and make it a space
		if(ch == '-')
		{
			result[i] = ' ';

			// Capitalize the character after space if possible
			if(i + 1 < result.size())
			{
				result[i + 1] = string_utils::az_to_AZ(result[i + 1]);
			}
		}
	}

	return result;
}

void generate_sdl_interface(const EInterfaceGenerator type, const std::string_view outputDirectoryStr)
{
	auto outputDirectory = Path(std::string(outputDirectoryStr));
	outputDirectory.createDirectory();

	if(!outputDirectory.hasDirectory())
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
		get_registered_sdl_classes(),
		get_registered_sdl_enums());

	PH_LOG(SdlGenApi, "done generating SDL interface, output to <{}>",
		generator->getOutputDirectory().toAbsoluteString());
}

}// end namespace ph::sdlgen