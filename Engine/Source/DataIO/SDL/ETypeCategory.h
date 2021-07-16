#pragma once

#include "Common/assertion.h"

#include <string>
#include <unordered_map>

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

namespace sdl
{

inline std::string category_to_string(const ETypeCategory category)
{
	std::string categoryName;
	switch(category)
	{
	case ETypeCategory::UNSPECIFIED:         categoryName = "unspecified";     break;
	case ETypeCategory::REF_GEOMETRY:        categoryName = "geometry";        break;
	case ETypeCategory::REF_MOTION:          categoryName = "motion";          break;
	case ETypeCategory::REF_MATERIAL:        categoryName = "material";        break;
	case ETypeCategory::REF_LIGHT_SOURCE:    categoryName = "light-source";    break;
	case ETypeCategory::REF_ACTOR:           categoryName = "actor";           break;
	case ETypeCategory::REF_FRAME_PROCESSOR: categoryName = "frame-processor"; break;
	case ETypeCategory::REF_IMAGE:           categoryName = "image";           break;
	case ETypeCategory::REF_OBSERVER:        categoryName = "observer";        break;
	case ETypeCategory::REF_SAMPLE_SOURCE:   categoryName = "sample-source";   break;
	case ETypeCategory::REF_VISUALIZER:      categoryName = "visualizer";      break;
	case ETypeCategory::REF_OPTION:          categoryName = "option";          break;

	default:
		// All categories must already have a string equivalent entry
		PH_ASSERT_UNREACHABLE_SECTION();
		break;
	}

	return categoryName;
}

inline ETypeCategory string_to_category(const std::string& categoryStr)
{
	const static std::unordered_map<std::string, ETypeCategory> map = 
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

}// end namespace sdl

}// end namespace ph
