#pragma once

#include "Common/assertion.h"
#include "Utility/string_utils_table.h"

#include <string>
#include <algorithm>
#include <string_view>

namespace ph::string_utils
{

enum class EWhitespace
{
	COMMON,
	STANDARD
};

template<EWhitespace TYPE = EWhitespace::COMMON>
inline std::string_view get_whitespace()
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
		PH_ASSERT_UNREACHABLE_SECTION();
		return "";
	}
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
	return cut_head(srcStr, get_whitespace<TYPE>());
}

/*! @brief Remove white spaces from the end.

@param srcStr String that is going to be trimmed.
@return The trimmed string.
*/
template<EWhitespace TYPE = EWhitespace::COMMON>
inline std::string_view trim_tail(const std::string_view srcStr)
{
	return cut_tail(srcStr, get_whitespace<TYPE>());
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
	return static_cast<char>(table::ASCII_TO_UPPER[ch]);
}

/*! @brief Convert upper-case characters to lower-case.

Characters that are not English alphabets, or being lower-case already,
will be preserved.
*/
inline char AZ_to_az(const char ch)
{
	return static_cast<char>(table::ASCII_TO_LOWER[ch]);
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

}// end namespace ph::string_utils
