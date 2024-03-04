#pragma once

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

}// end namespace ph
