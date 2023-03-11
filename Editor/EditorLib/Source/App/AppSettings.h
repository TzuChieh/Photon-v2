#pragma once

#include "RenderCore/EGraphicsAPI.h"

#include <Common/primitive_type.h>
#include <Math/TVector2.h>
#include <Utility/TSpan.h>

#include <string>

namespace ph::editor
{

// TODO: can be load/save from/to file, etc
class AppSettings final
{
public:
	std::string    title           = "Photon Editor";
	math::Vector2S displaySizePx   = {0, 0};
	uint32f        maxFPS          = 60;
	EGraphicsAPI   graphicsApi     = EGraphicsAPI::OpenGL;
	bool           useDebugModeGHI = false;
	//bool           useDebugModeGHI = true;

	TSpanView<const char*> cmdArgs;

	AppSettings(int argc, char* argv[]);
};

}// end namespace ph::editor
