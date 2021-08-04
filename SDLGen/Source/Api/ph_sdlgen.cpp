#include "ph_sdlgen.h"
#include "InterfaceGen/MarkdownDoc/MarkdownDocGenerator.h"

#include <Utility/string_utils.h>
#include <Common/Logger.h>
#include <Utility/utility.h>
#include <ph_cpp_core.h>

#include <cstddef>
#include <utility>

namespace ph::sdlgen
{

namespace
{

const Logger logger(LogSender("SDLGen"));

}

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

void generate_sdl_interface(const EInterfaceGenerator type, Path outputDirectory)
{
	auto generator = InterfaceGenerator::makeGenerator(type, std::move(outputDirectory));
	if(!generator)
	{
		logger.log(ELogLevel::WARNING_MED,
			"invalid generator type <" + enum_to_string(type) + "> provided, nothing is generated");
		return;
	}

	PH_ASSERT(generator);

	logger.log("generating SDL interface via generator " + generator->getName());

	generator->generate(
		get_registered_sdl_classes(),
		get_registered_sdl_enums());

	logger.log("done generating SDL interface, output to <" + generator->getOutputDirectory().toAbsoluteString() + ">");
}

}// end namespace ph::sdlgen
