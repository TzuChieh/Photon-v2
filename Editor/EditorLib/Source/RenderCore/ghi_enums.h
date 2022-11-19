#pragma once

#include "RenderCore/EGraphicsAPI.h"
#include "RenderCore/EClearTarget.h"

#include <cstddef>

namespace ph::editor
{

enum class EGHIPixelFormat
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

enum class EGHIPixelComponent
{
	LDR_8,
	HDR_16F,
	HDR_32F
};

enum class EGHIFilterMode
{
	Point,
	Linear
};

enum class EGHIWrapMode
{
	ClampToEdge,
	Repeat
};

std::size_t apparent_bytes_in_single_pixel(EGHIPixelFormat format);
std::size_t apparent_bytes_in_pixel_component(EGHIPixelComponent component);

}// end namespace ph::editor
