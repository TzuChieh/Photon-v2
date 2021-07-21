#pragma once

#include "EngineEnv/EAccelerator.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

PH_DEFINE_SDL_ENUM(TBasicSdlEnum<EAccelerator>)
{
	SdlEnumType sdlEnum("accelerator");
	sdlEnum.description("Denotes acceleration structure types.");

	sdlEnum.addEntry(EnumType::UNSPECIFIED,    "");
	sdlEnum.addEntry(EnumType::BRUTE_FORCE,    "brute-force");
	sdlEnum.addEntry(EnumType::BVH,            "bvh");
	sdlEnum.addEntry(EnumType::KDTREE,         "kd-tree");
	sdlEnum.addEntry(EnumType::INDEXED_KDTREE, "indexed-kd-tree");

	return sdlEnum;
}

}// end namespace ph
