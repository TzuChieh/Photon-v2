#pragma once

#include "Core/SampleGenerator/Halton/halton_randomization_enums.h"
#include "SDL/sdl_interface.h"

namespace ph
{

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<EHaltonPermutation>)
{
	SdlEnumType sdlEnum("halton-permutation");
	sdlEnum.description(
		"Different permutation schemes for Halton sample generator.");
	
	sdlEnum.addEntry(EnumType::None, "");
	sdlEnum.addEntry(EnumType::None, "none",
		"No permutation. This can exhibit great deal of aliasing since raw Halton sequence is "
		"actually fully deterministic.");

	sdlEnum.addEntry(EnumType::Fixed, "fixed",
		"Using the same permutation for all digits in a dimension. This is great for breaking up "
		"patterns while maintaining good performance. However, this scheme is not so uniformly "
		"random, see PBRT-v4, section 6 in Chapter 8.6.2.");

	sdlEnum.addEntry(EnumType::PerDigit, "per-digit",
		"Using different permutation for each digit in a dimension. Consumes more memory but "
		"better in breaking up patterns.");

	sdlEnum.addEntry(EnumType::Owen, "owen",
		"In addition to per-digit permutation, this scheme considers all previous digits when "
		"permuting each digit.");

	return sdlEnum;
}

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<EHaltonSequence>)
{
	SdlEnumType sdlEnum("halton-sequence");
	sdlEnum.description(
		"Different generated sequence of Halton sample generator.");
	
	sdlEnum.addEntry(EnumType::Original, "");
	sdlEnum.addEntry(EnumType::Original, "original",
		"The original sequence starting from sample index 0.");

	sdlEnum.addEntry(EnumType::Leap, "leap",
		"Skipping a pre-defined amount of samples between each sample. The skip amount is carefully "
		"chosen to not introduce degenerated samples.");

	sdlEnum.addEntry(EnumType::RandomLeap, "random-leap",
		"Similar to leap, except that a random leap amount is used for each sample generator "
		"life cycle.");

	sdlEnum.addEntry(EnumType::RandomStart, "random-start",
		"Choosing a starting point for each dimension randomly (for each sample generator "
		"life cycle). In practice (where we only have finite precision), this is actually skipping "
		"a certain amount of samples for each dimension.");

	return sdlEnum;
}

}// end namespace ph
