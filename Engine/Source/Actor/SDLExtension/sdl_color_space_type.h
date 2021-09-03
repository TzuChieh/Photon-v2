#pragma once

#include "Math/Color/EColorSpace.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

PH_DEFINE_SDL_ENUM(TBasicSdlEnum<math::EColorSpace>)
{
	SdlEnumType sdlEnum("color-space");
	sdlEnum.description("Marks color space information of input values.");

	sdlEnum.addEntry(EnumType::UNSPECIFIED, "");
	sdlEnum.addEntry(EnumType::LINEAR_SRGB, "LSRGB");
	sdlEnum.addEntry(EnumType::SRGB,        "SRGB");
	sdlEnum.addEntry(EnumType::SPECTRAL,    "SPD");

	return sdlEnum;
}

}// end namespace ph
