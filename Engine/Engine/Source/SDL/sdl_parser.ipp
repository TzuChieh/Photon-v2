#pragma once

#include "SDL/sdl_parser.h"

#include <Utility/string_utils.h>

namespace ph::sdl_parser
{

inline bool is_specifier(const char ch)
{
	return ch == persistent_specifier || ch == cached_specifier;
}

inline bool starts_with_specifier(std::string_view valueToken)
{
	valueToken = string_utils::trim_head(valueToken);
	return !valueToken.empty() && is_specifier(valueToken[0]);
} 

}// end namespace ph::sdl_parser
