#pragma once

#include "SDL/sdl_interface.h"

namespace ph
{

enum class ESampleFilter
{
	Unspecified = 0,

	Box,
	Gaussian,
	MitchellNetravali,
	BlackmanHarris
};

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<ESampleFilter>)
{
	SdlEnumType sdlEnum("sample-filter");
	sdlEnum.description("The type of filter used during the sampling process.");
	
	sdlEnum.addEntry(EnumType::Unspecified, "");

	sdlEnum.addEntry(EnumType::Box, "box",
		"The box filter. Fairly sharp, fast to evaluate, but can have obvious aliasing around edges.");

	sdlEnum.addEntry(EnumType::Gaussian, "gaussian",
		"The Gaussian filter. Gives smooth results, slower to evaluate and can sometimes result in blurry images.");

	sdlEnum.addEntry(EnumType::MitchellNetravali, "mn",
		"The Mitchell-Netravali filter. Smooth but remains sharp around edges.");

	sdlEnum.addEntry(EnumType::BlackmanHarris, "bh",
		"The Blackman-Harris filter. A good compromise between smoothness and sharpness.");

	return sdlEnum;
}

}// end namespace ph
