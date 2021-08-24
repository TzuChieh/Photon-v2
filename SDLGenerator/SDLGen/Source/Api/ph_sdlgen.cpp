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

std::string sdl_name_to_title_case(const std::string_view sdlName)
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

std::string sdl_name_to_camel_case(const std::string_view sdlName, const bool capitalizedFront)
{
	// The implementation is aware of empty inputs

	std::string camelCase = sdl_name_to_title_case(sdlName);
	string_utils::erase_all(camelCase, ' ');

	if(!capitalizedFront && !camelCase.empty())
	{
		camelCase.front() = string_utils::AZ_to_az(camelCase.front());
	}

	return camelCase;
}

std::string sdl_name_to_snake_case(const std::string_view sdlName)
{
	// The implementation is aware of empty inputs

	std::string snakeCase = sdl_name_to_title_case(sdlName);

	for(std::size_t i = 0; i < snakeCase.size(); ++i)
	{
		// Replace each space in title with dash
		if(snakeCase[i] == ' ')
		{
			snakeCase[i] = '_';
		}

		// Make A~Z characters lower-case
		snakeCase[i] = string_utils::AZ_to_az(snakeCase[i]);
	}

	return snakeCase;
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
