#pragma once

#include <Common/assertion.h>

#include <string_view>
#include <unordered_map>
#include <array>
#include <cstddef>
#include <vector>

namespace ph
{

enum class ESdlTypeCategory
{
	Unspecified = 0,

	Ref_Geometry, 
	Ref_Material,
	Ref_Motion,
	Ref_Image,
	Ref_Actor, 
	Ref_FrameProcessor,
	Ref_Observer,
	Ref_SampleSource,
	Ref_Visualizer,
	Ref_Option,
	Ref_Object,
	
	// Special values (these entries should always be the last ones)
	SIZE
};

namespace sdl::detail
{

// Must match the entries in `ESdlTypeCategory`
inline constexpr std::array<std::string_view, static_cast<std::size_t>(ESdlTypeCategory::SIZE)> CATEGORY_TO_STRING =
{{
	"unspecified",
	"geometry",
	"material",
	"motion",
	"image",
	"actor",
	"frame-processor",
	"observer",
	"sample-source",
	"visualizer",
	"option",
	"object"
}};

}// end namespace sdl::detail

namespace sdl
{

inline std::string_view category_to_string(const ESdlTypeCategory category)
{
	const auto categoryIndex = static_cast<std::size_t>(category);
	PH_ASSERT_LT(categoryIndex, detail::CATEGORY_TO_STRING.size());
	return detail::CATEGORY_TO_STRING[categoryIndex];
}

inline ESdlTypeCategory string_to_category(const std::string_view categoryStr)
{
	const static std::unordered_map<std::string_view, ESdlTypeCategory> map =
	{ 
		{category_to_string(ESdlTypeCategory::Ref_Geometry),       ESdlTypeCategory::Ref_Geometry},
		{category_to_string(ESdlTypeCategory::Ref_Material),       ESdlTypeCategory::Ref_Material},
		{category_to_string(ESdlTypeCategory::Ref_Motion),         ESdlTypeCategory::Ref_Motion},
		{category_to_string(ESdlTypeCategory::Ref_Image),          ESdlTypeCategory::Ref_Image},
		{category_to_string(ESdlTypeCategory::Ref_Actor),          ESdlTypeCategory::Ref_Actor},
		{category_to_string(ESdlTypeCategory::Ref_FrameProcessor), ESdlTypeCategory::Ref_FrameProcessor},
		{category_to_string(ESdlTypeCategory::Ref_Observer),       ESdlTypeCategory::Ref_Observer},
		{category_to_string(ESdlTypeCategory::Ref_SampleSource),   ESdlTypeCategory::Ref_SampleSource},
		{category_to_string(ESdlTypeCategory::Ref_Visualizer),     ESdlTypeCategory::Ref_Visualizer},
		{category_to_string(ESdlTypeCategory::Ref_Option),         ESdlTypeCategory::Ref_Option},
		{category_to_string(ESdlTypeCategory::Ref_Object),         ESdlTypeCategory::Ref_Object}

		// Any other strings including the string for `ESdlTypeCategory::Unspecified`
		// is not explicitly mapped here, as by default they all mapped to `ESdlTypeCategory::Unspecified`
	};

	const auto& iter = map.find(categoryStr);
	if(iter == map.end())
	{
		// Map to "unspecified" by default
		return ESdlTypeCategory::Unspecified;
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
