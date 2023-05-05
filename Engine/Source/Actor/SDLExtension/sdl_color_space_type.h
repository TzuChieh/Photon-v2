#pragma once

#include "Math/Color/color_enums.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<math::EColorSpace>)
{
	SdlEnumType sdlEnum("color-space");
	sdlEnum.description("Marks color space information of input values.");

	sdlEnum.addEntry(EnumType::Unspecified, "");
	sdlEnum.addEntry(EnumType::Linear_sRGB, "LSRGB");
	sdlEnum.addEntry(EnumType::sRGB,        "SRGB");
	sdlEnum.addEntry(EnumType::ACEScg,      "ACES");

	// Default spectral representation is always defined to be "SPD" on the SDL side, so
	// it is easier to change the mapped color space later in C++.
	sdlEnum.addEntry(EnumType::Spectral, "SPD");

	return sdlEnum;
}

}// end namespace ph
