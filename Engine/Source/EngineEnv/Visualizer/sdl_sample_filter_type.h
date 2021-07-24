#pragma once

#include "EngineEnv/Visualizer/ESampleFilter.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

PH_DEFINE_SDL_ENUM(TBasicSdlEnum<ESampleFilter>)
{
	SdlEnumType sdlEnum("sample-filter");
	sdlEnum.description("The type of filter used during the sampling process.");
	
	sdlEnum.addEntry(EnumType::UNSPECIFIED, "");

	sdlEnum.addEntry(EnumType::BOX, "box",
		"Fairly sharp, fast to evaluate, but can have obvious aliasing around edges.");

	sdlEnum.addEntry(EnumType::GAUSSIAN, "gaussian",
		"Gives smooth results, slower to evaluate and can sometimes result in blurry images.");

	sdlEnum.addEntry(EnumType::MITCHELL_NETRAVALI, "mn",
		"Smooth but remains sharp around edges.");

	sdlEnum.addEntry(EnumType::BLACKMAN_HARRIS, "bh",
		"A good compromise between smoothness and sharpness.");

	return sdlEnum;
}

}// end namespace ph
