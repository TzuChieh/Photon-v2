#pragma once

#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

enum class EInterfaceFresnel
{
	UNSPECIFIED = 0,

	SCHLICK,
	EXACT
};

PH_DEFINE_SDL_ENUM(TBasicSdlEnum<EInterfaceFresnel>)
{
	SdlEnumType sdlEnum("interface-fresnel");
	sdlEnum.description("Controls the Fresnel model used.");

	sdlEnum.addEntry(EnumType::UNSPECIFIED, "");
	sdlEnum.addEntry(EnumType::SCHLICK, "schlick", "An approximative model developed by Schlick.");
	sdlEnum.addEntry(EnumType::EXACT, "exact", "The full-form Fresnel formula.");

	return sdlEnum;
}

}// end namespace ph
