#pragma once

#include "RenderCore/EGraphicsAPI.h"
#include "RenderCore/EClearTarget.h"

namespace ph::editor
{

enum class EGHITextureFormat
{
	RGB_8,
	RGBA_8
};

enum class EGHITextureSampleMode
{
	Point,
	Linear
};

enum class EGHITextureWrapMode
{
	ClampToEdge,
	Repeat
};

}// end namespace ph::editor
