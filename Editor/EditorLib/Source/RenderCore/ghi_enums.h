#pragma once

#include "RenderCore/EGraphicsAPI.h"
#include "RenderCore/EClearTarget.h"

#include <Frame/picture_basics.h>
#include <Utility/traits.h>

#include <cstddef>

namespace ph::editor
{

/*! @brief Combined pixel layout and component type information.
Unless stated explicitly, the characters RGB does not mean the pixel is in RGB color space. 
It simply represents pixel components, using R, G, B as placeholders.
*/
enum class EGHIInfoPixelFormat : uint8
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

enum class EGHIInfoPixelComponent : uint8
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

enum class EGHIInfoFilterMode : uint8
{
	Point,
	Linear
};

enum class EGHIInfoWrapMode : uint8
{
	ClampToEdge,
	Repeat
};

enum class EGHIInfoStorageElement : uint8
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

enum class EGHIInfoStorageHint : uint8
{
	Unspecified = 0,
	Static,
	Dynamic
};

enum class EGHIInfoShadingStage : uint8
{
	Unspecified = 0,
	Vertex,
	Fragment
};

std::size_t num_bytes(EGHIInfoPixelFormat format);
std::size_t num_bytes(EGHIInfoPixelComponent component);
std::size_t num_bytes(EGHIInfoStorageElement element);

template<CIsEnum DstEnumType>
DstEnumType translate_to(EPicturePixelComponent pictureComponent);

}// end namespace ph::editor

#include "RenderCore/ghi_enums.ipp"
