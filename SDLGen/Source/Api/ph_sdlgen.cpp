#include "ph_sdlgen.h"

#include <Utility/string_utils.h>

#include <cstddef>

namespace ph::sdlgen
{

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

}// end namespace ph::sdlgen
