#pragma once

#include "Common/assertion.h"
#include "Utility/string_utils_table.h"

#include <string>
#include <algorithm>
#include <string_view>

namespace ph::string_utils
{

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

inline std::string_view cut_ends(const std::string_view srcStr, const char* const candidates)
{
	return cut_head(cut_tail(srcStr, candidates), candidates);
}

inline std::string_view trim_head(const std::string_view srcStr)
{
	return cut_head(srcStr, " \n\r\t");
}

inline std::string_view trim_tail(const std::string_view srcStr)
{
	return cut_tail(srcStr, " \n\r\t");
}

inline std::string_view trim(const std::string_view srcStr)
{
	return trim_head(trim_tail(srcStr));
}

inline void az_to_AZ(std::string* const str)
{
	PH_ASSERT(str);

	for(char& ch : *str)
	{
		ch = table::ASCII_TO_UPPER[static_cast<unsigned char>(ch)];
	}
}

inline void AZ_to_az(std::string* const str)
{
	PH_ASSERT(str);

	for(char& ch : *str)
	{
		ch = table::ASCII_TO_LOWER[static_cast<unsigned char>(ch)];
	}
}

}// end namespace ph::string_utils
