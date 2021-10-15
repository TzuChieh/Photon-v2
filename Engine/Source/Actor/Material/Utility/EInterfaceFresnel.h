#pragma once

#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

enum class EInterfaceFresnel
{
	UNSPECIFIED = 0,

	Schlick,
	Exact
};

PH_DEFINE_SDL_ENUM(TBasicSdlEnum<EInterfaceFresnel>)
{
	SdlEnumType sdlEnum("interface-fresnel");
	sdlEnum.description("Controls the Fresnel model used.");

	sdlEnum.addEntry(EnumType::UNSPECIFIED, "");
	sdlEnum.addEntry(EnumType::Schlick, "schlick", "An approximative model developed by Schlick.");
	sdlEnum.addEntry(EnumType::Exact, "exact", "The full-form Fresnel formula.");

	return sdlEnum;
}

}// end namespace ph
