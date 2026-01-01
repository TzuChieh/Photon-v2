#pragma once

#include "Engine/Math/Color/color_enums.h"
#include "Engine/SDL/sdl_interface.h"

namespace ph
{

PH_DEFINE_SDL_ENUM(math::EColorSpace, e)
{
	e.name("color-space");
	e.description("Marks color space information of input values.");

	e.addEntry(EnumType::Unspecified, "");
	e.addEntry(EnumType::CIE_XYZ,     "XYZ");
	e.addEntry(EnumType::CIE_xyY,     "xyY");
	e.addEntry(EnumType::Linear_sRGB, "LSRGB");
	e.addEntry(EnumType::sRGB,        "SRGB");
	e.addEntry(EnumType::ACEScg,      "ACEScg");

	// Spectral representation is always defined to be "SPD" on the SDL side and is expected to
	// have values for constructing a spectral distribution. This way, it is easier to change the
	// mapped color space later in C++.
	e.addEntry(EnumType::Spectral,    "SPD");
}

PH_DEFINE_SDL_ENUM(math::EColorUsage, e)
{
	e.name("color-usage");
	e.description("Marks color usage information of input values.");

	e.addEntry(EnumType::Unspecified, "");
	e.addEntry(EnumType::Raw,         "RAW");
	e.addEntry(EnumType::EMR,         "EMR");
	e.addEntry(EnumType::ECF,         "ECF");
}

}// end namespace ph
