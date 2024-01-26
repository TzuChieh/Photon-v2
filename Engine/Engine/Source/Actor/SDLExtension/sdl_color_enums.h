#pragma once

#include "Math/Color/color_enums.h"
#include "SDL/sdl_interface.h"

namespace ph
{

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<math::EColorSpace>)
{
	SdlEnumType sdlEnum("color-space");
	sdlEnum.description("Marks color space information of input values.");

	sdlEnum.addEntry(EnumType::Unspecified, "");
	sdlEnum.addEntry(EnumType::CIE_XYZ,     "XYZ");
	sdlEnum.addEntry(EnumType::CIE_xyY,     "xyY");
	sdlEnum.addEntry(EnumType::Linear_sRGB, "LSRGB");
	sdlEnum.addEntry(EnumType::sRGB,        "SRGB");
	sdlEnum.addEntry(EnumType::ACEScg,      "ACEScg");

	// Spectral representation is always defined to be "SPD" on the SDL side and is expected to
	// have values for constructing a spectral distribution. This way, it is easier to change the
	// mapped color space later in C++.
	sdlEnum.addEntry(EnumType::Spectral,    "SPD");

	return sdlEnum;
}

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<math::EColorUsage>)
{
	SdlEnumType sdlEnum("color-usage");
	sdlEnum.description("Marks color usage information of input values.");

	sdlEnum.addEntry(EnumType::Unspecified, "");
	sdlEnum.addEntry(EnumType::RAW,         "RAW");
	sdlEnum.addEntry(EnumType::EMR,         "EMR");
	sdlEnum.addEntry(EnumType::ECF,         "ECF");

	return sdlEnum;
}

}// end namespace ph
