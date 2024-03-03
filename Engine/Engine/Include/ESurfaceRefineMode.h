#pragma once

namespace ph
{

enum class ESurfaceRefineMode
{
	Manual,
	Empirical,
	Iterative,

	// Special values
	Default = Empirical,
};

}// end namespace ph
