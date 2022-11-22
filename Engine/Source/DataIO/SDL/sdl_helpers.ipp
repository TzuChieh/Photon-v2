#pragma once

#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/SDL/Tokenizer.h"
#include "Common/assertion.h"
#include "DataIO/SDL/sdl_traits.h"

#include <type_traits>

namespace ph::sdl
{

template<typename FloatType>
inline FloatType load_float(const std::string_view sdlFloatStr)
{
	try
	{
		return string_utils::parse_float<FloatType>(sdlFloatStr);
	}
	catch(const std::exception& e)
	{
		throw SdlLoadError("on loading floating-point value -> " + std::string(e.what()));
	}
}

template<typename IntType>
inline IntType load_int(const std::string_view sdlIntStr)
{
	try
	{
		return string_utils::parse_int<IntType>(sdlIntStr);
	}
	catch(const std::exception& e)
	{
		throw SdlLoadError("on loading integer value -> " + std::string(e.what()));
	}
}

template<typename NumberType>
inline NumberType load_number(const std::string_view sdlNumberStr)
{
	if constexpr(std::is_floating_point_v<NumberType>)
	{
		return load_float<NumberType>(sdlNumberStr);
	}
	else
	{
		static_assert(std::is_integral_v<NumberType>);

		return load_int<NumberType>(sdlNumberStr);
	}
}

inline real load_real(const std::string_view sdlRealStr)
{
	return load_float<real>(sdlRealStr);
}

inline integer load_integer(const std::string_view sdlIntegerStr)
{
	return load_int<integer>(sdlIntegerStr);
}

inline void save_real(const real value, std::string* const out_str)
{
	save_float<real>(value, out_str);
}

inline void save_integer(const integer value, std::string* const out_str)
{
	save_int<integer>(value, out_str);
}

template<typename FloatType>
inline void save_float(const FloatType value, std::string* const out_str, const std::size_t maxChars)
{
	try
	{
		string_utils::stringify_number(value, out_str, maxChars);
	}
	catch(const std::exception& e)
	{
		throw SdlSaveError("on saving floating-point value -> " + std::string(e.what()));
	}
}

template<typename IntType>
inline void save_int(const IntType value, std::string* const out_str, const std::size_t maxChars)
{
	try
	{
		string_utils::stringify_number(value, out_str, maxChars);
	}
	catch(const std::exception& e)
	{
		throw SdlSaveError("on saving integer value -> " + std::string(e.what()));
	}
}

template<typename NumberType>
inline void save_number(const NumberType value, std::string* const out_str, const std::size_t maxChars)
{
	if constexpr(std::is_floating_point_v<NumberType>)
	{
		save_float<NumberType>(value, out_str, maxChars);
	}
	else
	{
		static_assert(std::is_integral_v<NumberType>);

		save_int<NumberType>(value, out_str, maxChars);
	}
}

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

template<typename T>
inline constexpr ETypeCategory category_of()
{
	if constexpr(CIsSdlResource<T> && CHasStaticSdlCategoryInfo<T>)
	{
		return T::CATEGORY;
	}
	else
	{
		return ETypeCategory::UNSPECIFIED;
	}
}

}// end namespace ph::sdl
