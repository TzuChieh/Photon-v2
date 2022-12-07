#pragma once

#include "EngineEnv/EAccelerator.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

PH_DEFINE_SDL_ENUM(TBasicSdlEnum<EAccelerator>)
{
	SdlEnumType sdlEnum("accelerator");
	sdlEnum.description("Denotes acceleration structure types.");

	sdlEnum.addEntry(EnumType::Unspecified,   "");
	sdlEnum.addEntry(EnumType::BruteForce,    "brute-force");
	sdlEnum.addEntry(EnumType::BVH,           "bvh");
	sdlEnum.addEntry(EnumType::Kdtree,        "kd-tree");
	sdlEnum.addEntry(EnumType::IndexedKdtree, "indexed-kd-tree");

	return sdlEnum;
}

}// end namespace ph
