#pragma once

#include "EngineEnv/EAccelerator.h"
#include "SDL/sdl_interface.h"

namespace ph
{

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<EAccelerator>)
{
	SdlEnumType sdlEnum("accelerator");
	sdlEnum.description("Denotes acceleration structure types.");

	sdlEnum.addEntry(EnumType::Unspecified,   "");
	sdlEnum.addEntry(EnumType::BruteForce,    "brute-force");
	sdlEnum.addEntry(EnumType::BVH,           "bvh");
	sdlEnum.addEntry(EnumType::BVH4,          "bvh4");
	sdlEnum.addEntry(EnumType::Kdtree,        "kd-tree");
	sdlEnum.addEntry(EnumType::IndexedKdtree, "indexed-kd-tree");

	return sdlEnum;
}

}// end namespace ph
