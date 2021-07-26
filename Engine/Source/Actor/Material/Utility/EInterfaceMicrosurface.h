#pragma once

#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

enum class EInterfaceMicrosurface
{
	UNSPECIFIED = 0,

	TROWBRIDGE_REITZ,// a.k.a. GGX
	BECKMANN
};

PH_DEFINE_SDL_ENUM(TBasicSdlEnum<EInterfaceMicrosurface>)
{
	SdlEnumType sdlEnum("interface-microsurface");
	sdlEnum.description("Controls the model for describing the micro structure of the interface.");

	sdlEnum.addEntry(EnumType::UNSPECIFIED, "");
	sdlEnum.addEntry(EnumType::TROWBRIDGE_REITZ, "ggx", "Formally known as the Trowbridge-Reitz distribution.");
	sdlEnum.addEntry(EnumType::BECKMANN, "beckmann", "The Beckmann distribution.");

	return sdlEnum;
}

}// end namespace ph
