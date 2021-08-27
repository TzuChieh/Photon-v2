#pragma once

#include "Common/assertion.h"
#include "Utility/string_utils_table.h"

#include <cstddef>
#include <string>
#include <algorithm>
#include <string_view>
#include <stdexcept>
#include <charconv>
#include <limits>
#include <type_traits>

namespace ph::string_utils
{

enum class EWhitespace
{
	COMMON,
	STANDARD
};

template<EWhitespace TYPE = EWhitespace::COMMON>
inline std::string_view get_whitespaces()
{
	if constexpr(TYPE == EWhitespace::COMMON)
	{
		return table::common_whitespaces;
	}
	else if constexpr(TYPE == EWhitespace::STANDARD)
	{
		return table::standard_whitespaces;
	}
	else
	{
		static_assert(TYPE == EWhitespace::COMMON || TYPE == EWhitespace::STANDARD,
			"Must include a case for each enum entry; did you forget to add one?");
		
		return "";
	}
}

template<EWhitespace TYPE = EWhitespace::COMMON>
inline constexpr bool is_whitespace(const char ch)
{
	return get_whitespaces<TYPE>().find(ch) != std::string_view::npos;
}

/*! @brief Remove characters from the beginning.

Characters in @p srcStr will be removed from the beginning if they match any
of the character in @p candidates. The process stops once a mismatch is encountered.

@param srcStr String that is going to be cut.
@param candidates The character set used to remove characters from @p srcStr.
@return The cut string.
*/
inline std::string_view cut_head(const std::string_view srcStr, const std::string_view candidates)
{
	const auto nonCutPos = srcStr.find_first_not_of(candidates);

	auto cutStr = srcStr;

	// remove_prefix(): behavior is undefined for inputPos > size(), avoid that
	// with the ternary operator
	cutStr.remove_prefix(
		nonCutPos != std::string_view::npos ? nonCutPos : srcStr.size());

	return cutStr;
}

/*! @brief Remove characters from the end.

Characters in @p srcStr will be removed from the end if they match any
of the character in @p candidates. The process stops once a mismatch is encountered.

@param srcStr String that is going to be cut.
@param candidates The character set used to remove characters from @p srcStr.
@return The cut string.
*/
inline std::string_view cut_tail(const std::string_view srcStr, const std::string_view candidates)
{
	const auto nonCutPos = srcStr.find_last_not_of(candidates);

	auto cutStr = srcStr;

	// remove_suffix(): behavior is undefined for inputPos > size(), avoid that
	// with the ternary operator;
	// also, if <nonCutPos> is not npos, <srcStr> will not be empty
	cutStr.remove_suffix(
		nonCutPos != std::string_view::npos ? srcStr.size() - 1 - nonCutPos : srcStr.size());

	return cutStr;
}

/*! @brief Remove characters from both ends.

Characters in @p srcStr will be removed from both ends if they match any
of the character in @p candidates. The process stops once a mismatch is encountered.

@param srcStr String that is going to be cut.
@param candidates The character set used to remove characters from @p srcStr.
@return The cut string.
*/
inline std::string_view cut_ends(const std::string_view srcStr, const std::string_view candidates)
{
	return cut_head(cut_tail(srcStr, candidates), candidates);
}

/*! @brief Remove white spaces from the beginning.

@param srcStr String that is going to be trimmed.
@return The trimmed string.
*/
template<EWhitespace TYPE = EWhitespace::COMMON>
inline std::string_view trim_head(const std::string_view srcStr)
{
	return cut_head(srcStr, get_whitespaces<TYPE>());
}

/*! @brief Remove white spaces from the end.

@param srcStr String that is going to be trimmed.
@return The trimmed string.
*/
template<EWhitespace TYPE = EWhitespace::COMMON>
inline std::string_view trim_tail(const std::string_view srcStr)
{
	return cut_tail(srcStr, get_whitespaces<TYPE>());
}

/*! @brief Remove white spaces from both ends.

@param srcStr String that is going to be trimmed.
@return The trimmed string.
*/
template<EWhitespace TYPE = EWhitespace::COMMON>
inline std::string_view trim(const std::string_view srcStr)
{
	return trim_head<TYPE>(trim_tail<TYPE>(srcStr));
}

/*! @brief Convert lower-case characters to upper-case.

Characters that are not English alphabets, or being upper-case already,
will be preserved.
*/
inline char az_to_AZ(const char ch)
{
	static_assert(std::numeric_limits<unsigned char>::max() == table::ASCII_TO_UPPER.size() - 1);

	const auto mappedCharIdx = static_cast<unsigned char>(ch);
	return static_cast<char>(table::ASCII_TO_UPPER[mappedCharIdx]);
}

/*! @brief Convert upper-case characters to lower-case.

Characters that are not English alphabets, or being lower-case already,
will be preserved.
*/
inline char AZ_to_az(const char ch)
{
	static_assert(std::numeric_limits<unsigned char>::max() == table::ASCII_TO_LOWER.size() - 1);

	const auto mappedCharIdx = static_cast<unsigned char>(ch);
	return static_cast<char>(table::ASCII_TO_LOWER[mappedCharIdx]);
}

/*! @brief Convert lower-case characters to upper-case.

Characters that are not English alphabets, or being upper-case already, 
will be preserved.

@param str String that is going to be converted in-place.
*/
inline void az_to_AZ(std::string* const str)
{
	PH_ASSERT(str);

	for(char& ch : *str)
	{
		ch = az_to_AZ(ch);
	}
}

/*! @brief Convert upper-case characters to lower-case.

Characters that are not English alphabets, or being lower-case already, 
will be preserved.

@param str String that is going to be converted in-place.
*/
inline void AZ_to_az(std::string* const str)
{
	PH_ASSERT(str);

	for(char& ch : *str)
	{
		ch = AZ_to_az(ch);
	}
}

/*! @brief Repeat the input string for N times.
*/
inline std::string repeat(const std::string_view str, const std::size_t n)
{
	const std::size_t totalSize = str.size() * n;

	// Valid for the case where <totalSize> is 0
	std::string result;
	result.reserve(totalSize);
	for(std::size_t i = 0; i < n; ++i)
	{
		result += str;
	}

	return result;
}

/*! @brief Remove all occurrence of a character in the string.
*/
inline void erase_all(std::string& str, const char ch)
{
	str.erase(std::remove(str.begin(), str.end(), ch), str.end());
}

namespace detail
{

inline void throw_from_std_errc_if_has_error(const std::errc errorCode)
{
	// According to several sources, 0, or zero-initialized std::errc,
	// indicates no error.
	//
	// [1] see the example for std::from_chars
	//     https://en.cppreference.com/w/cpp/utility/from_chars
	// [2] https://stackoverflow.com/a/63567008
	//
	constexpr std::errc NO_ERROR_VALUE = std::errc();

	switch(errorCode)
	{
	case NO_ERROR_VALUE:
		return;

	case std::errc::invalid_argument:
		throw std::invalid_argument("invalid argument");

	case std::errc::result_out_of_range:
		throw std::out_of_range("result out of range");

	default:
		throw std::runtime_error(
			"unknown error: std::errc = " + std::to_string(
			static_cast<std::underlying_type_t<std::errc>>(errorCode)));
	}
}

}// end namespace detail

/*! @brief Returns a float by processing its string representation.

Supports float, double, and long double.
*/
template<typename T>
inline T parse_float(const std::string_view floatStr)
{
	static_assert(std::is_floating_point_v<T>,
		"parse_float() accepts only floating point type.");

	// FIXME: looks like in VS 15.9.16 from_chars() cannot parse str with
	// leading whitespaces while it should be able to auto skip them, we
	// do it manually for now:
	const std::string_view floatStrNoLeadingWS = trim_head(floatStr);

	T value;
	const std::from_chars_result result = std::from_chars(
		floatStrNoLeadingWS.data(),
		floatStrNoLeadingWS.data() + floatStrNoLeadingWS.size(),
		value);

	/*T value;
	const std::from_chars_result result = std::from_chars(
		sdlFloatStr.data(),
		sdlFloatStr.data() + sdlFloatStr.size(),
		value);*/

	detail::throw_from_std_errc_if_has_error(result.ec);

	return value;
}

/*! @brief Returns an integer by processing its string representation.

Supports all signed and unsigned standard integer types.
*/
template<typename T>
inline T parse_int(const std::string_view intStr)
{
	static_assert(std::is_integral_v<T>,
		"parse_int() accepts only integer type.");

	// FIXME: looks like in VS 15.9.16 from_chars() cannot parse str with
	// leading whitespaces while it should be able to auto skip them, we
	// do it manually for now:
	const std::string_view intStrNoLeadingWS = trim_head(intStr);

	T value;
	const std::from_chars_result result = std::from_chars(
		intStrNoLeadingWS.data(),
		intStrNoLeadingWS.data() + intStrNoLeadingWS.size(),
		value);

	/*T value;
	const std::from_chars_result result = std::from_chars(
		sdlIntegerStr.data(),
		sdlIntegerStr.data() + sdlIntegerStr.size(),
		value);*/

	detail::throw_from_std_errc_if_has_error(result.ec);

	return value;
}

/*! @brief Returns a number by processing its string representation.

Accepts all types supported by parse_float(std::string_view) and parse_int(std::string_view).
*/
template<typename NumberType>
inline NumberType parse_number(const std::string_view numberStr)
{
	if constexpr(std::is_floating_point_v<NumberType>)
	{
		return parse_float<NumberType>(numberStr);
	}
	else
	{
		static_assert(std::is_integral_v<NumberType>);

		return parse_int<NumberType>(numberStr);
	}
}

/*! @brief Converts a float to string.

Supports float, double, and long double. The function expects a large enough 
@p bufferSize determined by the caller.

@param out_buffer The buffer for storing the string.
@param bufferSize Size of @p out_buffer.
@return Size of the string written to @p out_buffer.
*/
//template<typename T>
//inline std::size_t stringify_float(const T value, char* const out_buffer, const std::size_t bufferSize)
//{
//	static_assert(std::is_floating_point_v<T>,
//		"stringify_float() accepts only floating point type.");
//
//	PH_ASSERT(out_buffer);
//	PH_ASSERT_GE(bufferSize, 1);
//
//	// TODO
//
//	// FIXME: looks like in VS 15.9.16 from_chars() cannot parse str with
//	// leading whitespaces while it should be able to auto skip them, we
//	// do it manually for now:
//	const std::string_view floatStrNoLeadingWS = trim_head(floatStr);
//
//	T value;
//	const std::from_chars_result result = std::from_chars(
//		floatStrNoLeadingWS.data(),
//		floatStrNoLeadingWS.data() + floatStrNoLeadingWS.size(),
//		value);
//
//	/*T value;
//	const std::from_chars_result result = std::from_chars(
//		sdlFloatStr.data(),
//		sdlFloatStr.data() + sdlFloatStr.size(),
//		value);*/
//
//	detail::throw_from_std_errc_if_has_error(result.ec);
//
//	return value;
//}

}// end namespace ph::string_utils
