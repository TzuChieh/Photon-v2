#pragma once

#include "EditorLib/RenderCore/EGraphicsAPI.h"

#include <Engine/SDL/sdl_interface.h>

namespace ph
{

PH_DEFINE_SDL_ENUM(editor::ghi::EGraphicsAPI, e)
{
	e.name("graphics-api");
	e.description("Type of the hardware rendering API.");

	e.addEntry(EnumType::Unknown, "");
	e.addEntry(EnumType::OpenGL,  "OPENGL");
}

}// end namespace ph
