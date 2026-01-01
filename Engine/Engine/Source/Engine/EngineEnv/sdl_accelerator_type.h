#pragma once

#include "Engine/EngineEnv/EAccelerator.h"
#include "Engine/SDL/sdl_interface.h"

namespace ph
{

PH_DEFINE_SDL_ENUM(EAccelerator, e)
{
	e.name("accelerator");
	e.description("Denotes acceleration structure types.");

	e.addEntry(EnumType::Unspecified,   "");
	e.addEntry(EnumType::BruteForce,    "brute-force");
	e.addEntry(EnumType::BVH,           "bvh");
	e.addEntry(EnumType::BVH4,          "bvh4");
	e.addEntry(EnumType::BVH8,          "bvh8");
	e.addEntry(EnumType::Kdtree,        "kd-tree");
	e.addEntry(EnumType::IndexedKdtree, "indexed-kd-tree");
}

}// end namespace ph
