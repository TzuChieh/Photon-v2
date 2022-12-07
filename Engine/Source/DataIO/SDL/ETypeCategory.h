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
	Unspecified = 0,

	Ref_Geometry, 
	Ref_Material,
	Ref_Motion,
	Ref_LightSource, 
	Ref_Actor, 
	Ref_Image,
	Ref_FrameProcessor,
	Ref_Observer,
	Ref_SampleSource,
	Ref_Visualizer,
	Ref_Option,
	
	// Special values (these entries should always be the last ones)
	MIN = Unspecified,
	MAX = Ref_Option,
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
		{category_to_string(ETypeCategory::Ref_Geometry),       ETypeCategory::Ref_Geometry},
		{category_to_string(ETypeCategory::Ref_Material),       ETypeCategory::Ref_Material},
		{category_to_string(ETypeCategory::Ref_Motion),         ETypeCategory::Ref_Motion},
		{category_to_string(ETypeCategory::Ref_LightSource),    ETypeCategory::Ref_LightSource},
		{category_to_string(ETypeCategory::Ref_Actor),          ETypeCategory::Ref_Actor},
		{category_to_string(ETypeCategory::Ref_Observer),       ETypeCategory::Ref_Observer},
		{category_to_string(ETypeCategory::Ref_Image),          ETypeCategory::Ref_Image},
		{category_to_string(ETypeCategory::Ref_SampleSource),   ETypeCategory::Ref_SampleSource},
		{category_to_string(ETypeCategory::Ref_FrameProcessor), ETypeCategory::Ref_FrameProcessor},
		{category_to_string(ETypeCategory::Ref_Visualizer),     ETypeCategory::Ref_Visualizer},
		{category_to_string(ETypeCategory::Ref_Option),         ETypeCategory::Ref_Option}

		// Any other strings including the string for `ETypeCategory::Unspecified`
		// is not explicitly mapped here, as by default they all mapped to `ETypeCategory::Unspecified`
	};

	const auto& iter = map.find(categoryStr);
	if(iter == map.end())
	{
		// Map to "unspecified" by default
		return ETypeCategory::Unspecified;
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
