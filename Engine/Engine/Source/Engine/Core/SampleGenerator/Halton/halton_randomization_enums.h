#pragma once

namespace ph
{

/*! @brief Permutation schemes available for a Halton sequence.
See `sdl_halton_randomization_types.h` for detailed explanation.
*/
enum class EHaltonPermutation
{
	None = 0,
	Fixed,
	PerDigit,
	Owen
};

enum class EHaltonSequence
{
	Original = 0,
	Leap,
	RandomLeap,
	RandomStart
};

}// end namespace ph::halton
