#pragma once

#include "Math/Color/color_enums.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

PH_DEFINE_SDL_ENUM(TBasicSdlEnum<math::EColorUsage>)
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
