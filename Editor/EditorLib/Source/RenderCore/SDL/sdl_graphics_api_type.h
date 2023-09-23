#pragma once

#include "RenderCore/EGraphicsAPI.h"

#include <SDL/sdl_interface.h>

namespace ph
{

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<editor::ghi::EGraphicsAPI>)
{
	SdlEnumType sdlEnum("graphics-api");
	sdlEnum.description("Type of the hardware rendering API.");

	sdlEnum.addEntry(EnumType::Unknown,   "");
	sdlEnum.addEntry(EnumType::OpenGL,    "OPENGL");

	return sdlEnum;
}

}// end namespace ph
