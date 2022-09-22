#pragma once

#include "EGraphicsAPI.h"

#include <Common/primitive_type.h>
#include <Math/TVector2.h>

#include <string>

namespace ph::editor
{

// TODO: can be load/save from/to file, etc
class AppSettings final
{
public:
	std::string    title         = "Photon Editor";
	math::Vector2S displaySizePx = {0, 0};
	uint32f        maxFPS        = 60;
	EGraphicsAPI   graphicsApi   = EGraphicsAPI::OpenGL;
};

}// end namespace ph::editor
