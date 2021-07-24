#pragma once

#include "EngineEnv/Visualizer/ERayEnergyEstimator.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

PH_DEFINE_SDL_ENUM(TBasicSdlEnum<ERayEnergyEstimator>)
{
	SdlEnumType sdlEnum("estimator");
	sdlEnum.description("Type of energy estimation algorithms.");
	
	sdlEnum.addEntry(EnumType::UNSPECIFIED, "");

	sdlEnum.addEntry(EnumType::BVPT, "bvpt",
		"Backward path tracing.");

	sdlEnum.addEntry(EnumType::BNEEPT, "bneept",
		"Backward path tracing with next event estimation.");

	sdlEnum.addEntry(EnumType::BVPTDL, "bvptdl",
		"Backward path tracing, evaluate direct lighting only (single bounce)");

	return sdlEnum;
}

}// end namespace ph
