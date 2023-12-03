#pragma once

#include "SDL/sdl_interface.h"

namespace ph
{

enum class ERayEnergyEstimator
{
	Unspecified = 0,

	BVPT,
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

	sdlEnum.addEntry(EnumType::BNEEPT, "bneept",
		"Backward path tracing with next event estimation.");

	sdlEnum.addEntry(EnumType::BVPTDL, "bvptdl",
		"Backward path tracing, evaluate direct lighting only (single bounce)");

	return sdlEnum;
}

}// end namespace ph
