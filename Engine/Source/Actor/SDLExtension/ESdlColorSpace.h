#pragma once

#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

enum class ESdlColorSpace
{
	UNSPECIFIED = 0,

	LINEAR_SRGB,
	SRGB,
	SPECTRAL,

	// Special values (these entries should always be the last ones)
	MIN = UNSPECIFIED,
	MAX = SPECTRAL,
	NUM = MAX - MIN + 1
};

PH_DEFINE_SDL_ENUM(TBasicSdlEnum<ESdlColorSpace, ESdlColorSpace::NUM>)
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
