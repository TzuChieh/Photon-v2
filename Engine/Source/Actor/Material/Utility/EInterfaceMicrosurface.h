#pragma once

#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

enum class EInterfaceMicrosurface
{
	Unspecified = 0,

	TrowbridgeReitz,// a.k.a. GGX
	Beckmann
};

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<EInterfaceMicrosurface>)
{
	SdlEnumType sdlEnum("interface-microsurface");
	sdlEnum.description("Controls the model for describing the micro structure of the interface.");

	sdlEnum.addEntry(EnumType::Unspecified, "");
	sdlEnum.addEntry(EnumType::TrowbridgeReitz, "ggx", "Formally known as the Trowbridge-Reitz distribution.");
	sdlEnum.addEntry(EnumType::Beckmann, "beckmann", "The Beckmann distribution.");

	return sdlEnum;
}

}// end namespace ph
