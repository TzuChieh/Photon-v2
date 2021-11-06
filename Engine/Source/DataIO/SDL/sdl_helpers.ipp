#pragma once

#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/SDL/Tokenizer.h"
#include "Common/assertion.h"

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

template<typename NumberType>
inline std::vector<NumberType> load_number_array(const std::string& sdlNumberArrayStr)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	try
	{
		std::vector<std::string> arrayTokens;
		tokenizer.tokenize(sdlNumberArrayStr, arrayTokens);

		std::vector<NumberType> numberArray(arrayTokens.size());
		for(std::size_t i = 0; i < numberArray.size(); ++i)
		{
			numberArray[i] = load_number<NumberType>(arrayTokens[i]);
		}

		return numberArray;
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError("on parsing number array -> " + e.whatStr());
	}
}

template<typename Element>
inline void save_vector2(const math::TVector2<Element>& value, std::string* const out_str)
{
	PH_ASSERT(out_str);

	try
	{
		if(value.x() == value.y())
		{
			save_number<Element>(value.x(), out_str);
		}
		else
		{
			out_str->clear();

			std::string savedElement;

			(*out_str) += '\"';
			save_number<Element>(value.x(), &savedElement);
			(*out_str) += savedElement;
			(*out_str) += ' ';
			save_number<Element>(value.y(), &savedElement);
			(*out_str) += savedElement;
			(*out_str) += '\"';
		}
	}
	catch(const SdlSaveError& e)
	{
		throw SdlSaveError("on saving Vector2 -> " + e.whatStr());
	}
}

template<typename NumberType>
inline void save_number_array(const std::vector<NumberType>& values, std::string* const out_str)
{
	PH_ASSERT(out_str);
		
	out_str->clear();

	try
	{
		(*out_str) += '\"';

		std::string savedNumber;
		for(const auto& value : values)
		{
			save_number<NumberType>(value, &savedNumber);
			(*out_str) += savedNumber;
			(*out_str) += ' ';
		}

		(*out_str) += '\"';
	}
	catch(const SdlSaveError& e)
	{
		throw SdlSaveError("on saving number array -> " + e.whatStr());
	}
}

}// end namespace ph::sdl
