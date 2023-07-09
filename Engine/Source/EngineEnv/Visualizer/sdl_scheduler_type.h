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
	sdlEnum.addEntry(EnumType::Bulk,        "bulk");
	sdlEnum.addEntry(EnumType::Stripe,      "stripe");
	sdlEnum.addEntry(EnumType::Grid,        "grid");
	sdlEnum.addEntry(EnumType::Tile,        "tile");
	sdlEnum.addEntry(EnumType::Spiral,      "spiral");
	sdlEnum.addEntry(EnumType::SpiralGrid,  "spiral-grid");

	return sdlEnum;
}

}// end namespace ph
