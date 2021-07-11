#pragma once

#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/SDL/Tokenizer.h"

namespace ph::sdl
{

template<typename Element>
inline math::TVector2<Element> load_vector2(const std::string& sdlVector2Str)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	try
	{
		std::vector<std::string> tokens;
		tokenizer.tokenize(sdlVector2Str, tokens);

		// 2 input values form a vec2 exactly
		if(tokens.size() == 2)
		{
			return math::TVector2<Element>(
				load_number<Element>(tokens[0]),
				load_number<Element>(tokens[1]));
		}
		// 1 input value results in vec2 filled with the same value
		else if(tokens.size() == 1)
		{
			return math::TVector2<Element>(
				load_number<Element>(tokens[0]));
		}
		else
		{
			throw SdlLoadError(
				"invalid Vector2 representation "
				"(number of values = " + std::to_string(tokens.size()) + ")");
		}
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError("on parsing Vector2 -> " + e.whatStr());
	}
}

}// end namespace ph::sdl
