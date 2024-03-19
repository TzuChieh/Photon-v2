#pragma once

#include <Common/assertion.h>

#include <string_view>

namespace ph
{

enum class ESurfaceRefineMode
{
	Manual,
	Empirical,
	Iterative,

	// Special values
	Default = Iterative,
};

inline std::string_view refine_mode_to_str(const ESurfaceRefineMode mode)
{
	switch(mode)
	{
	case ESurfaceRefineMode::Manual: return "manual";
	case ESurfaceRefineMode::Empirical: return "empirical";
	case ESurfaceRefineMode::Iterative: return "iterative";

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return "";
	}
}

inline ESurfaceRefineMode str_to_refine_mode(std::string_view mode)
{
	if(mode == "manual")
	{
		return ESurfaceRefineMode::Manual;
	}
	else if(mode == "empirical")
	{
		return ESurfaceRefineMode::Empirical;
	}
	else if(mode == "iterative")
	{
		return ESurfaceRefineMode::Iterative;
	}
	else if(mode == "default")
	{
		return ESurfaceRefineMode::Default;
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
		return ESurfaceRefineMode::Default;
	}
}

}// end namespace ph
