#pragma once

#include "Engine/Core/SampleGenerator/Halton/halton_randomization_enums.h"
#include "Engine/SDL/sdl_interface.h"

namespace ph
{

PH_DEFINE_SDL_ENUM(EHaltonPermutation, e)
{
	e.name("halton-permutation");
	e.description(
		"Different permutation schemes for Halton sample generator.");
	
	e.addEntry(EnumType::None, "");
	e.addEntry(EnumType::None, "none",
		"No permutation. This can exhibit great deal of aliasing since raw Halton sequence is "
		"actually fully deterministic.");

	e.addEntry(EnumType::Fixed, "fixed",
		"Using the same permutation for all digits in a dimension. This is great for breaking up "
		"patterns while maintaining good performance. However, this scheme is not so uniformly "
		"random, see PBRT-v4, section 6 in Chapter 8.6.2.");

	e.addEntry(EnumType::PerDigit, "per-digit",
		"Using different permutation for each digit in a dimension. Consumes more memory but "
		"better in breaking up patterns.");

	e.addEntry(EnumType::Owen, "owen",
		"In addition to per-digit permutation, this scheme considers all previous digits when "
		"permuting each digit.");
}

PH_DEFINE_SDL_ENUM(EHaltonSequence, e)
{
	e.name("halton-sequence");
	e.description(
		"Different generated sequence of Halton sample generator.");
	
	e.addEntry(EnumType::Original, "");
	e.addEntry(EnumType::Original, "original",
		"The original sequence starting from sample index 0.");

	e.addEntry(EnumType::Leap, "leap",
		"Skipping a pre-defined amount of samples between each sample. The skip amount is carefully "
		"chosen to not introduce degenerated samples.");

	e.addEntry(EnumType::RandomLeap, "random-leap",
		"Similar to leap, except that a random leap amount is used for each sample generator "
		"life cycle.");

	e.addEntry(EnumType::RandomStart, "random-start",
		"Choosing a starting point for each dimension randomly (for each sample generator "
		"life cycle). In practice (where we only have finite precision), this is actually skipping "
		"a certain amount of samples for each dimension.");
}

}// end namespace ph
