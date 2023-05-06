#pragma once

#include "Core/Scheduler/EScheduler.h"
#include "SDL/sdl_interface.h"

namespace ph
{

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<EScheduler>)
{
	SdlEnumType sdlEnum("scheduler");
	sdlEnum.description("Denotes work distribution types.");
	
	sdlEnum.addEntry(EnumType::Unspecified, "");
	sdlEnum.addEntry(EnumType::BULK,        "bulk");
	sdlEnum.addEntry(EnumType::STRIPE,      "stripe");
	sdlEnum.addEntry(EnumType::GRID,        "grid");
	sdlEnum.addEntry(EnumType::TILE,        "tile");
	sdlEnum.addEntry(EnumType::SPIRAL,      "spiral");
	sdlEnum.addEntry(EnumType::SPIRAL_GRID, "spiral-grid");

	return sdlEnum;
}

}// end namespace ph
