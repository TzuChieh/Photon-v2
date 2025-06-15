#pragma once

#include "Engine/Core/Scheduler/EScheduler.h"
#include "Engine/Core/LTA/SidednessAgreement.h"

#include "Engine/SDL/sdl_interface.h"

namespace ph
{

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<EScheduler>)
{
	SdlEnumType sdlEnum("scheduler");
	sdlEnum.description("Denotes work distribution types.");
	
	sdlEnum.addEntry(EnumType::Unspecified, "");
	sdlEnum.addEntry(EnumType::Bulk,        "bulk");
	sdlEnum.addEntry(EnumType::Stripe,      "stripe");
	sdlEnum.addEntry(EnumType::Grid,        "grid");
	sdlEnum.addEntry(EnumType::Tile,        "tile");
	sdlEnum.addEntry(EnumType::Spiral,      "spiral");
	sdlEnum.addEntry(EnumType::SpiralGrid,  "spiral-grid");

	return sdlEnum;
}

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<lta::ESidednessPolicy>)
{
	SdlEnumType sdlEnum("sidedness-policy");
	sdlEnum.description("Policy to use when judging front and back faces.");
	
	sdlEnum.addEntry(EnumType::DoNotCare,     "");
	sdlEnum.addEntry(EnumType::DoNotCare,     "do-not-care");
	sdlEnum.addEntry(EnumType::Strict,        "strict");
	sdlEnum.addEntry(EnumType::TrustGeometry, "trust-geometry");
	sdlEnum.addEntry(EnumType::TrustShading,  "trust-shading");

	return sdlEnum;
}

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

enum class ESampleFilter
{
	Unspecified = 0,

	Box,
	Gaussian,
	MitchellNetravali,
	BlackmanHarris
};

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<ESampleFilter>)
{
	SdlEnumType sdlEnum("sample-filter");
	sdlEnum.description("The type of filter used during the sampling process.");
	
	sdlEnum.addEntry(EnumType::Unspecified, "");

	sdlEnum.addEntry(EnumType::Box, "box",
		"The box filter. Fairly sharp, fast to evaluate, but can have obvious aliasing around edges.");

	sdlEnum.addEntry(EnumType::Gaussian, "gaussian",
		"The Gaussian filter. Gives smooth results, slower to evaluate and can sometimes result in blurry images.");

	sdlEnum.addEntry(EnumType::MitchellNetravali, "mn",
		"The Mitchell-Netravali filter. Smooth but remains sharp around edges.");

	sdlEnum.addEntry(EnumType::BlackmanHarris, "bh",
		"The Blackman-Harris filter. A good compromise between smoothness and sharpness.");

	return sdlEnum;
}

enum class ERayEnergyEstimator
{
	Unspecified = 0,

	BVPT,
	BVVPT,
	BNEEPT,
	BVPTDL
};

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<ERayEnergyEstimator>)
{
	SdlEnumType sdlEnum("estimator");
	sdlEnum.description("Type of energy estimation algorithms.");
	
	sdlEnum.addEntry(EnumType::Unspecified, "");

	sdlEnum.addEntry(EnumType::BVPT, "bvpt",
		"Backward path tracing.");

	sdlEnum.addEntry(EnumType::BVVPT, "bvvpt",
		"Backward path tracing, with volumetric light transport.");

	sdlEnum.addEntry(EnumType::BNEEPT, "bneept",
		"Backward path tracing with next event estimation.");

	sdlEnum.addEntry(EnumType::BVPTDL, "bvptdl",
		"Backward path tracing, evaluate direct lighting only (single bounce)");

	return sdlEnum;
}

}// end namespace ph
