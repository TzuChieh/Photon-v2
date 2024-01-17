#pragma once

#include "SDL/sdl_interface.h"

namespace ph
{

enum class EPhotonMappingMode
{
	Vanilla = 0,
	Progressive,
	StochasticProgressive,
	ProbabilisticProgressive
};

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<EPhotonMappingMode>)
{
	SdlEnumType sdlEnum("photon-mapping-mode");
	sdlEnum.description("Type of the photon mapping method.");
	
	sdlEnum.addEntry(EnumType::Vanilla, "vanilla",
		"Directly compute energy values from photon map, no fancy tricks applied.");

	sdlEnum.addEntry(EnumType::Progressive, "progressive",
		"Progressively refine the rendered results.");

	sdlEnum.addEntry(EnumType::StochasticProgressive, "stochastic-progressive",
		"Stochastic sampling technique is utilized for energy value computation.");

	sdlEnum.addEntry(EnumType::ProbabilisticProgressive, "probabilistic-progressive",
		"Probabilistic approach to progressive photon mapping. Consumes more memory, but is "
		"easily parallelizable to multiple workers.");

	return sdlEnum;
}

}// end namespace ph
