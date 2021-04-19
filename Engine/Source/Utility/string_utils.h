#pragma once

#include "Common/assertion.h"

#include <string_view>
#include <algorithm>

namespace ph::string_utils
{

inline std::string_view cut_head(const std::string_view srcStr, const char* const candidates)
{
	PH_ASSERT(candidates);

	const auto nonCutPos = srcStr.find_first_not_of(candidates);

	auto cutStr = srcStr;

	// remove_prefix(): behavior is undefined for input > size()
	cutStr.remove_prefix(
		nonCutPos != std::string_view::npos ? nonCutPos : srcStr.size());

	return cutStr;
}

inline std::string_view cut_tail(const std::string_view srcStr, const char* const candidates)
{
	PH_ASSERT(candidates);

	const auto nonCutPos = srcStr.find_last_not_of(candidates);

	auto cutStr = srcStr;

	// remove_suffix(): behavior is undefined for input > size();
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

}// end namespace ph::string_utils
