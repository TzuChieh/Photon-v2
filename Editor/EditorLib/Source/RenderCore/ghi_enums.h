#pragma once

#include "RenderCore/EGraphicsAPI.h"
#include "RenderCore/EClearTarget.h"

#include <Frame/picture_basics.h>

#include <cstddef>

namespace ph::editor
{

/*! @brief Combined pixel layout and component type information.
Unless stated explicitly, the characters RGB does not mean the pixel is in RGB color space. 
It simply represents pixel components, using R, G, B as placeholders.
*/
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
	Empty = 0,

	Int8,
	UInt8,
	Int16,
	UInt16,
	Int32,
	UInt32,
	Int64,
	UInt64,
	Float16,
	Float32,
	Float64
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
EGHIInfoPixelComponent from_picture_pixel_component(EPicturePixelComponent component);

}// end namespace ph::editor
