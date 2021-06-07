#pragma once

#include "Common/assertion.h"
#include "Utility/string_utils_table.h"

#include <string>
#include <algorithm>
#include <string_view>

namespace ph::string_utils
{

/*! @brief Remove characters from the beginning.

@param srcStr String that is going to be cut.
@param candidates Characters in @p srcStr will be removed until any of @p candidates are met.
@return The cut string.
*/
inline std::string_view cut_head(const std::string_view srcStr, const char* const candidates)
{
	PH_ASSERT(candidates);

	const auto nonCutPos = srcStr.find_first_not_of(candidates);

	auto cutStr = srcStr;

	// remove_prefix(): behavior is undefined for input > size(), avoid that
	// with the ternary operator
	cutStr.remove_prefix(
		nonCutPos != std::string_view::npos ? nonCutPos : srcStr.size());

	return cutStr;
}

/*! @brief Remove characters from the end.

@param srcStr String that is going to be cut.
@param candidates Characters in @p srcStr will be removed until any of @p candidates are met.
@return The cut string.
*/
inline std::string_view cut_tail(const std::string_view srcStr, const char* const candidates)
{
	PH_ASSERT(candidates);

	const auto nonCutPos = srcStr.find_last_not_of(candidates);

	auto cutStr = srcStr;

	// remove_suffix(): behavior is undefined for input > size(), avoid that
	// with the ternary operator;
	// also, if <nonCutPos> is not npos, <srcStr> will not be empty
	cutStr.remove_suffix(
		nonCutPos != std::string_view::npos ? srcStr.size() - 1 - nonCutPos : srcStr.size());

	return cutStr;
}

/*! @brief Remove characters from both ends.

@param srcStr String that is going to be cut.
@param candidates Characters in @p srcStr will be removed until any of @p candidates are met.
@return The cut string.
*/
inline std::string_view cut_ends(const std::string_view srcStr, const char* const candidates)
{
	return cut_head(cut_tail(srcStr, candidates), candidates);
}

/*! @brief Remove white spaces from the beginning.

@param srcStr String that is going to be trimmed.
@return The trimmed string.
*/
inline std::string_view trim_head(const std::string_view srcStr)
{
	return cut_head(srcStr, " \n\r\t");
}

/*! @brief Remove white spaces from the end.

@param srcStr String that is going to be trimmed.
@return The trimmed string.
*/
inline std::string_view trim_tail(const std::string_view srcStr)
{
	return cut_tail(srcStr, " \n\r\t");
}

/*! @brief Remove white spaces from both ends.

@param srcStr String that is going to be trimmed.
@return The trimmed string.
*/
inline std::string_view trim(const std::string_view srcStr)
{
	return trim_head(trim_tail(srcStr));
}

/*! @brief Convert lower-case characters to upper-case.

Characters that are not English alphabets will be preserved.

@param str String that is going to be converted in-place.
*/
inline void az_to_AZ(std::string* const str)
{
	PH_ASSERT(str);

	for(char& ch : *str)
	{
		ch = static_cast<char>(table::ASCII_TO_UPPER[ch]);
	}
}

/*! @brief Convert upper-case characters to lower-case.

Characters that are not English alphabets will be preserved.

@param str String that is going to be converted in-place.
*/
inline void AZ_to_az(std::string* const str)
{
	PH_ASSERT(str);

	for(char& ch : *str)
	{
		ch = static_cast<char>(table::ASCII_TO_LOWER[ch]);
	}
}

}// end namespace ph::string_utils
