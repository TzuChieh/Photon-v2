#pragma once

#include "SDL/sdl_parser.h"

#include <Utility/string_utils.h>

namespace ph::sdl_parser
{

inline bool starts_with_specifier(std::string_view nameToken)
{
	nameToken = string_utils::trim(nameToken);
	return !nameToken.empty() && (nameToken[0] == '@' || nameToken[0] == '$');
}

}// end namespace ph::sdl_parser
