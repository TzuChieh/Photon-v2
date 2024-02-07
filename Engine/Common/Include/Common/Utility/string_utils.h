#pragma once

/*! @file
@brief String manipulation helpers.
*/

#include "Common/Utility/string_utils_table.h"
#include "Common/assertion.h"
#include "Common/exception.h"

#include <cstddef>
#include <string>
#include <algorithm>
#include <string_view>
#include <stdexcept>
#include <charconv>
#include <limits>
#include <climits>
#include <type_traits>
#include <format>
#include <concepts>
#include <array>

namespace ph::string_utils
{

template<typename ObjType>
concept CHasToString = requires (const ObjType& obj)
{
	{ obj.toString() } -> std::convertible_to<std::string_view>;
};

}// end namespace ph::string_utils

#define PH_DEFINE_INLINE_TO_STRING_FORMATTER_SPECIALIZATION(...)\
	struct ::std::formatter<__VA_ARGS__> : ::std::formatter<::std::string>\
	{\
		static_assert(::ph::string_utils::CHasToString<__VA_ARGS__>,\
			"type " #__VA_ARGS__ " must have a const method toString() and the result should be "\
			"implicitly convertible to std::string"); \
	\
		/* `parse()` is inherited from the base class */\
	\
		/* Define `format()` by calling `std::string`'s implementation with custom type's `toString()`*/\
		inline auto format(const __VA_ARGS__& value, ::std::format_context& ctx) const\
		{\
			return ::std::formatter<::std::string>::format(\
				value.toString(), ctx);\
		}\
	}

/*! @brief Defines a formatter which calls the `toString()` method.
For example, to define a `toString()` formatter for the class `SomeType`, place the macro after 
class definition:

@code
	class SomeType { (class definitions) };

	PH_DEFINE_INLINE_TO_STRING_FORMATTER(SomeType);
@endcode

@param ... The type to define a formatter for.
*/
#define PH_DEFINE_INLINE_TO_STRING_FORMATTER(...)\
	template<>\
	PH_DEFINE_INLINE_TO_STRING_FORMATTER_SPECIALIZATION(__VA_ARGS__)

/*! @brief Defines a formatter template which calls the `toString()` method.
For example, to define a `toString()` formatter for the class template `TSomeType`, place the macro 
after class definition:

@code
	template<typename T>
	class TSomeType { (class definitions) };

	template<typename T>
	PH_DEFINE_INLINE_TO_STRING_FORMATTER_TEMPLATE(TSomeType<T>);
@endcode

@param ... The type to define a formatter for.
*/
#define PH_DEFINE_INLINE_TO_STRING_FORMATTER_TEMPLATE(...)\
	PH_DEFINE_INLINE_TO_STRING_FORMATTER_SPECIALIZATION(__VA_ARGS__)

namespace ph::string_utils
{

enum class EWhitespace
{
	/*! Smaller set of whitespace characters that are often seen (see `table::common_whitespaces`). */
	Common,

	/*! Complete set of whitespace characters (see `table::standard_whitespaces`). */
	Standard
};

template<EWhitespace TYPE = EWhitespace::Common>
inline std::string_view get_whitespaces()
{
	if constexpr(TYPE == EWhitespace::Common)
	{
		return table::common_whitespaces;
	}
	else if constexpr(TYPE == EWhitespace::Standard)
	{
		return table::standard_whitespaces;
	}
	else
	{
		static_assert(TYPE == EWhitespace::Common || TYPE == EWhitespace::Standard,
			"Must include a case for each enum entry; did you forget to add one?");
		
		return "";
	}
}

template<EWhitespace TYPE = EWhitespace::Common>
inline constexpr bool is_whitespace(const char ch)
{
	return get_whitespaces<TYPE>().find(ch) != std::string_view::npos;
}

inline bool has_any_of(const std::string_view srcStr, const std::string_view candidates)
{
	const auto foundPos = srcStr.find_first_of(candidates);
	return foundPos != std::string_view::npos;
}

inline bool has_none_of(const std::string_view srcStr, const std::string_view candidates)
{
	return !has_any_of(srcStr, candidates);
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
template<EWhitespace TYPE = EWhitespace::Common>
inline std::string_view trim_head(const std::string_view srcStr)
{
	return cut_head(srcStr, get_whitespaces<TYPE>());
}

/*! @brief Remove white spaces from the end.

@param srcStr String that is going to be trimmed.
@return The trimmed string.
*/
template<EWhitespace TYPE = EWhitespace::Common>
inline std::string_view trim_tail(const std::string_view srcStr)
{
	return cut_tail(srcStr, get_whitespaces<TYPE>());
}

/*! @brief Remove white spaces from both ends.

@param srcStr String that is going to be trimmed.
@return The trimmed string.
*/
template<EWhitespace TYPE = EWhitespace::Common>
inline std::string_view trim(const std::string_view srcStr)
{
	return trim_head<TYPE>(trim_tail<TYPE>(srcStr));
}

/*! @brief Retrieve a token from a string.

@param srcStr The string that token is going to be retrieved from.
@param[out] out_remainingStr If not null, stores the string with the retrieved token and its separator
removed. Pointing to @p srcStr is valid, e.g., `next_token(str, &str)`.
@param tokenSeparators Charactors that separate the tokens. Defaults to whitespace characters.
*/
inline std::string_view next_token(
	std::string_view        srcStr, 
	std::string_view* const out_remainingStr = nullptr,
	const std::string_view  tokenSeparators = get_whitespaces<>())
{
	srcStr = cut_head(srcStr, tokenSeparators);

	const auto separatorPos = srcStr.find_first_of(tokenSeparators);
	if(separatorPos != std::string_view::npos)
	{
		const auto nextToken = srcStr.substr(0, separatorPos);
		if(out_remainingStr)
		{
			// `separatorPos + 1` as we do not want to include the separator
			*out_remainingStr = srcStr.substr(separatorPos + 1);
		}

		return nextToken;
	}
	else
	{
		return srcStr;
	}
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

namespace detail_from_to_char
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
		throw InvalidArgumentException(
			"input cannot be interpreted as a numeric value");

	case std::errc::result_out_of_range:
		throw OverflowException(
			"result will overflow the arithmetic type");

	case std::errc::value_too_large:
		throw OutOfRangeException(
			"result cannot fit in the output buffer");

	default:
		throw RuntimeException(
			"unknown error: std::errc = " + std::to_string(
			static_cast<std::underlying_type_t<std::errc>>(errorCode)));
	}
}

}// end namespace detail_from_to_char

/*! @brief Returns a float by processing its string representation.
Supports float, double, and long double.
*/
template<typename T>
inline T parse_float(const std::string_view floatStr)
{
	static_assert(std::is_floating_point_v<T>,
		"parse_float() accepts only floating point type.");

	// `std::from_chars()` do not ignore leading whitespaces, we need to do it manually
	const std::string_view floatStrNoLeadingWS = trim_head(floatStr);

	T value;
	const std::from_chars_result result = std::from_chars(
		floatStrNoLeadingWS.data(),
		floatStrNoLeadingWS.data() + floatStrNoLeadingWS.size(),
		value);

	detail_from_to_char::throw_from_std_errc_if_has_error(result.ec);

	return value;
}

/*! @brief Returns an integer by processing its string representation.
Supports the following:
1. Supports all signed and unsigned standard integer types.
2. Supports both base 10 (no prefix) and base 16 (0x prefix) inputs.
*/
template<typename T>
inline T parse_int(std::string_view intStr)
{
	// TODO: option to handle base prefix (e.g., 0x)

	static_assert(std::is_integral_v<T>,
		"parse_int() accepts only integer type.");

	// `std::from_chars()` do not ignore leading whitespaces, we need to do it manually
	intStr = trim_head(intStr);

	int base = 10;
	if(intStr.starts_with("0x"))
	{
		base = 16;

		// Remove "0x" as `std::from_chars()` do not recognize base prefix
		intStr.remove_prefix(2);
	}

	T value;
	const std::from_chars_result result = std::from_chars(
		intStr.data(),
		intStr.data() + intStr.size(),
		value,
		base);

	detail_from_to_char::throw_from_std_errc_if_has_error(result.ec);

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

Supports all built-in floating point types (e.g., float, double, and long double).
The function expects a large enough @p bufferSize determined by the caller.
The written string is not null terminated. By default, the stringified float guarantees round-trip
conversion--feeding the converted string `s` from `value` to `parse_float()` will result in the
same value.

@param out_buffer The buffer for storing the string.
@param bufferSize Size of @p out_buffer.
@return Number of characters written to @p out_buffer.
*/
template<typename T>
inline std::size_t stringify_float(const T value, char* const out_buffer, const std::size_t bufferSize)
{
	// TODO: option to handle base prefix (e.g., 0x)
	// TODO: option to handle precision

	static_assert(std::is_floating_point_v<T>,
		"stringify_float() accepts only floating point type.");

	PH_ASSERT(out_buffer);
	PH_ASSERT_GE(bufferSize, 1);

	const std::to_chars_result result = std::to_chars(
		out_buffer,
		out_buffer + bufferSize,
		value);

	detail_from_to_char::throw_from_std_errc_if_has_error(result.ec);

	// Must written at least a char, and must not exceed bufferSize
	PH_ASSERT(out_buffer < result.ptr && result.ptr <= out_buffer + bufferSize);
	return static_cast<std::size_t>(result.ptr - out_buffer);
}

/*! @brief Converts an integer to base [2, 62] string.

Supports all signed and unsigned standard integer types. The function expects a large enough
@p bufferSize determined by the caller. The written string is not null terminated.

@param out_buffer The buffer for storing the string.
@param bufferSize Size of @p out_buffer.
@return Number of characters written to @p out_buffer.
*/
template<std::integral T>
inline std::size_t stringify_int_alphabetic(
	T value, 
	char* const out_buffer, 
	const std::size_t bufferSize,
	const int base)
{
	PH_ASSERT(out_buffer);
	PH_ASSERT_IN_RANGE_INCLUSIVE(base, 2, 62);

	std::size_t numCharsWritten = 0;

	// Write sign
	if constexpr(std::is_signed_v<T>)
	{
		if(value < 0)
		{
			if(bufferSize == 0)
			{
				throw OutOfRangeException(
					"result cannot fit in the output buffer: 0 buffer size, cannot hold the negative sign");
			}

			out_buffer[0] = '-';
			++numCharsWritten;

			value = -value;
		}
	}

	// Use a temporary buffer, enough to hold base 2 output
	std::array<unsigned char, sizeof(T) * CHAR_BIT> tmpBuffer;
	auto tmpBufferEnd = tmpBuffer.end();

	PH_ASSERT_GE(value, 0);
	do
	{
		*(--tmpBufferEnd) = table::BASE62_DIGITS[value % base];
		value /= base;
	} while(value > 0);
	
	auto numDigits = tmpBuffer.end() - tmpBufferEnd;
	if(numCharsWritten + numDigits > bufferSize)
	{
		throw_formatted<OutOfRangeException>(
			"result cannot fit in the output buffer: need={}, given={}",
			numCharsWritten + numDigits, bufferSize);
	}
	else
	{
		std::copy(tmpBufferEnd, tmpBuffer.end(), out_buffer + numCharsWritten);
		numCharsWritten += numDigits;
	}

	return numCharsWritten;
}

/*! @brief Converts an integer to string.

Supports all signed and unsigned standard integer types. The function expects a large enough
@p bufferSize determined by the caller. The written string is not null terminated.

@param out_buffer The buffer for storing the string.
@param bufferSize Size of @p out_buffer.
@return Number of characters written to @p out_buffer.
*/
template<std::integral T>
inline std::size_t stringify_int(
	T value, 
	char* out_buffer, 
	std::size_t bufferSize,
	int base = 10)
{
	PH_ASSERT_IN_RANGE_INCLUSIVE(base, 2, 62);

	// Base in [2, 36] is supported by STL via `to_chars()`
	if(2 <= base && base <= 36)
	{
		PH_ASSERT(out_buffer);
		PH_ASSERT_GE(bufferSize, 1);

		std::to_chars_result result = std::to_chars(
			out_buffer,
			out_buffer + bufferSize,
			value,
			base);

		detail_from_to_char::throw_from_std_errc_if_has_error(result.ec);

		// Must written at least a char, and must not exceed bufferSize
		PH_ASSERT(out_buffer < result.ptr && result.ptr <= out_buffer + bufferSize);
		return static_cast<std::size_t>(result.ptr - out_buffer);
	}
	else
	{
		return stringify_int_alphabetic(value, out_buffer, bufferSize, base);
	}
}

/*! @brief Converts a number to string.
Accepts all types supported by stringify_float(T, char*, std::size_t) and
stringify_int(T, char*, std::size_t). The written string is not null terminated.
*/
template<typename NumberType>
inline std::size_t stringify_number(
	const NumberType value, 
	char* const out_buffer, 
	const std::size_t bufferSize)
{
	if constexpr(std::is_floating_point_v<NumberType>)
	{
		return stringify_float<NumberType>(value, out_buffer, bufferSize);
	}
	else
	{
		static_assert(std::is_integral_v<NumberType>);

		return stringify_int<NumberType>(value, out_buffer, bufferSize);
	}
}

/*! @brief Converts a number to string.
Similar to `stringify_number(T, char*, std::size_t)`, except that this variant writes to `std::string`
and the resulting string is guaranteed to be null terminated (by calling `std::string::c_str()`).
*/
template<typename NumberType>
inline std::string& stringify_number(
	const NumberType value, 
	std::string* const out_str, 
	const std::size_t maxChars = 64)
{
	PH_ASSERT(out_str);

	out_str->resize(maxChars);
	const std::size_t actualStrSize = string_utils::stringify_number<NumberType>(
		value, out_str->data(), out_str->size());
	out_str->resize(actualStrSize);
	return *out_str;
}

}// end namespace ph::string_utils
