#pragma once

#include "RenderCore/EGraphicsAPI.h"
#include "RenderCore/EClearTarget.h"

#include <cstddef>

namespace ph::editor
{

enum class EGHIInfoPixelFormat
{
	Empty = 0,

	RGB_8,
	RGBA_8,
	RGB_16F,
	RGBA_16F,
	RGB_32F,
	RGBA_32F,
	Depth_24_Stencil_8
};

enum class EGHIInfoPixelComponent
{
	LDR_8,
	HDR_16F,
	HDR_32F
};

enum class EGHIInfoFilterMode
{
	Point,
	Linear
};

enum class EGHIInfoWrapMode
{
	ClampToEdge,
	Repeat
};

std::size_t apparent_bytes_in_single_pixel(EGHIInfoPixelFormat format);
std::size_t apparent_bytes_in_pixel_component(EGHIInfoPixelComponent component);

}// end namespace ph::editor
