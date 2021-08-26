#pragma once

#include "Common/assertion.h"

#include <string_view>
#include <unordered_map>
#include <array>
#include <cstddef>
#include <vector>

namespace ph
{

enum class ETypeCategory
{
	UNSPECIFIED = 0,

	REF_GEOMETRY, 
	REF_MATERIAL,
	REF_MOTION,
	REF_LIGHT_SOURCE, 
	REF_ACTOR, 
	REF_IMAGE,
	REF_FRAME_PROCESSOR,
	REF_OBSERVER,
	REF_SAMPLE_SOURCE,
	REF_VISUALIZER,
	REF_OPTION,
	
	// Special values (these entries should always be the last ones)
	MIN = UNSPECIFIED,
	MAX = REF_OPTION,
	NUM = MAX - MIN + 1
};

namespace sdl::detail
{

// Must match the entries in ETypeCategory
inline constexpr std::array<std::string_view, static_cast<std::size_t>(ETypeCategory::NUM)> CATEGORY_TO_STRING = 
{{
	"unspecified",
	"geometry",
	"material",
	"motion",
	"light-source",
	"actor",
	"image",
	"frame-processor",
	"observer",
	"sample-source",
	"visualizer",
	"option"
}};

}// end namespace sdl::detail

namespace sdl
{

inline std::string category_to_string(const ETypeCategory category)
{
	const auto categoryIndex = static_cast<std::size_t>(category);
	PH_ASSERT_LT(categoryIndex, detail::CATEGORY_TO_STRING.size());
	return std::string(detail::CATEGORY_TO_STRING[categoryIndex]);
}

inline ETypeCategory string_to_category(const std::string_view categoryStr)
{
	const static std::unordered_map<std::string_view, ETypeCategory> map = 
	{ 
		{category_to_string(ETypeCategory::REF_GEOMETRY),        ETypeCategory::REF_GEOMETRY},
		{category_to_string(ETypeCategory::REF_MATERIAL),        ETypeCategory::REF_MATERIAL},
		{category_to_string(ETypeCategory::REF_MOTION),          ETypeCategory::REF_MOTION},
		{category_to_string(ETypeCategory::REF_LIGHT_SOURCE),    ETypeCategory::REF_LIGHT_SOURCE},
		{category_to_string(ETypeCategory::REF_ACTOR),           ETypeCategory::REF_ACTOR},
		{category_to_string(ETypeCategory::REF_OBSERVER),        ETypeCategory::REF_OBSERVER},
		{category_to_string(ETypeCategory::REF_IMAGE),           ETypeCategory::REF_IMAGE},
		{category_to_string(ETypeCategory::REF_SAMPLE_SOURCE),   ETypeCategory::REF_SAMPLE_SOURCE},
		{category_to_string(ETypeCategory::REF_FRAME_PROCESSOR), ETypeCategory::REF_FRAME_PROCESSOR},
		{category_to_string(ETypeCategory::REF_VISUALIZER),      ETypeCategory::REF_VISUALIZER},
		{category_to_string(ETypeCategory::REF_OPTION),          ETypeCategory::REF_OPTION}

		// Any other strings including the string for ETypeCategory::UNSPECIFIED
		// is not explicitly mapped here, as by default they all mapped to ETypeCategory::UNSPECIFIED
	};

	const auto& iter = map.find(categoryStr);
	if(iter == map.end())
	{
		// Map to "unspecified" by default
		return ETypeCategory::UNSPECIFIED;
	}

	return iter->second;
}

/*! @brief Returns a list of available categories.
This function is not intended to be used in performance critical code paths.
*/
inline std::vector<std::string_view> acquire_categories()
{
	return std::vector<std::string_view>(
		detail::CATEGORY_TO_STRING.begin(), 
		detail::CATEGORY_TO_STRING.end());
}

}// end namespace sdl

}// end namespace ph
