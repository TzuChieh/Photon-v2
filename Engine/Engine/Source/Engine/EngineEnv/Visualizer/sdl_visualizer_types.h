#pragma once

#include "Engine/Core/Scheduler/EScheduler.h"
#include "Engine/Core/LTA/SidednessAgreement.h"

#include "Engine/SDL/sdl_interface.h"

namespace ph
{

PH_DEFINE_SDL_ENUM(EScheduler, e)
{
	e.name("scheduler");
	e.description("Denotes work distribution types.");
	
	e.addEntry(EnumType::Unspecified, "");
	e.addEntry(EnumType::Bulk,        "bulk");
	e.addEntry(EnumType::Stripe,      "stripe");
	e.addEntry(EnumType::Grid,        "grid");
	e.addEntry(EnumType::Tile,        "tile");
	e.addEntry(EnumType::Spiral,      "spiral");
	e.addEntry(EnumType::SpiralGrid,  "spiral-grid");
}

PH_DEFINE_SDL_ENUM(lta::ESidednessPolicy, e)
{
	e.name("sidedness-policy");
	e.description("Policy to use when judging front and back faces.");
	
	e.addEntry(EnumType::DoNotCare,     "");
	e.addEntry(EnumType::DoNotCare,     "do-not-care");
	e.addEntry(EnumType::Strict,        "strict");
	e.addEntry(EnumType::TrustGeometry, "trust-geometry");
	e.addEntry(EnumType::TrustShading,  "trust-shading");
}

enum class EPhotonMappingMode
{
	Vanilla = 0,
	Progressive,
	StochasticProgressive,
	ProbabilisticProgressive
};

PH_DEFINE_SDL_ENUM(EPhotonMappingMode, e)
{
	e.name("photon-mapping-mode");
	e.description("Type of the photon mapping method.");
	
	e.addEntry(EnumType::Vanilla, "vanilla",
		"Directly compute energy values from photon map, no fancy tricks applied.");

	e.addEntry(EnumType::Progressive, "progressive",
		"Progressively refine the rendered results.");

	e.addEntry(EnumType::StochasticProgressive, "stochastic-progressive",
		"Stochastic sampling technique is utilized for energy value computation.");

	e.addEntry(EnumType::ProbabilisticProgressive, "probabilistic-progressive",
		"Probabilistic approach to progressive photon mapping. Consumes more memory, but is "
		"easily parallelizable to multiple workers.");
}

enum class ESampleFilter
{
	Unspecified = 0,

	Box,
	Gaussian,
	MitchellNetravali,
	BlackmanHarris
};

PH_DEFINE_SDL_ENUM(ESampleFilter, e)
{
	e.name("sample-filter");
	e.description("The type of filter used during the sampling process.");
	
	e.addEntry(EnumType::Unspecified, "");

	e.addEntry(EnumType::Box, "box",
		"The box filter. Fairly sharp, fast to evaluate, but can have obvious aliasing around edges.");

	e.addEntry(EnumType::Gaussian, "gaussian",
		"The Gaussian filter. Gives smooth results, slower to evaluate and can sometimes result in blurry images.");

	e.addEntry(EnumType::MitchellNetravali, "mn",
		"The Mitchell-Netravali filter. Smooth but remains sharp around edges.");

	e.addEntry(EnumType::BlackmanHarris, "bh",
		"The Blackman-Harris filter. A good compromise between smoothness and sharpness.");
}

enum class EFilm
{
	Beauty,
	Variance
};

PH_DEFINE_SDL_ENUM(EFilm, e)
{
	e.name("film");
	e.description("Type of film output.");

	e.addEntry(EnumType::Beauty, "beauty",
		"Standard color output.");

	e.addEntry(EnumType::Variance, "variance",
		"Per-channel (standard color) sample variance output.");
}

enum class ERayEnergyEstimator
{
	Unspecified = 0,

	BVPT,
	BVVPT,
	BNEEPT,
	BVPTDL
};

PH_DEFINE_SDL_ENUM(ERayEnergyEstimator, e)
{
	e.name("estimator");
	e.description("Type of energy estimation algorithms.");
	
	e.addEntry(EnumType::Unspecified, "");

	e.addEntry(EnumType::BVPT, "bvpt",
		"Backward path tracing.");

	e.addEntry(EnumType::BVVPT, "bvvpt",
		"Backward path tracing, with volumetric light transport.");

	e.addEntry(EnumType::BNEEPT, "bneept",
		"Backward path tracing with next event estimation.");

	e.addEntry(EnumType::BVPTDL, "bvptdl",
		"Backward path tracing, evaluate direct lighting only (single bounce)");
}

}// end namespace ph
