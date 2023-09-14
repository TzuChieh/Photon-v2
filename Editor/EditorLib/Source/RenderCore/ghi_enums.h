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
enum class EGHISizedPixelFormat : uint8
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

enum class EGHIPixelFormat : uint8
{
	Empty = 0,
	R,
	RG,
	RGB,
	RGBA,
	Depth,
	Stencil
};

enum class EGHIPixelComponent : uint8
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

enum class EGHIFilterMode : uint8
{
	Point,
	Linear
};

enum class EGHIWrapMode : uint8
{
	ClampToEdge,
	Repeat
};

enum class EGHIStorageElement : uint8
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

enum class EGHIStorageUsage : uint8
{
	Unspecified = 0,
	Static,
	Dynamic
};

enum class EGHIMeshDrawMode : uint8
{
	Points,
	LineSegments,
	LineCurveOpened,
	LineCurveClosed,
	TriangleStrip,
	TriangleFan,
	Triangles
};

enum class EGHIShadingStage : uint8
{
	Unspecified = 0,
	Vertex,
	Fragment,
	Compute
};

std::size_t num_bytes(EGHISizedPixelFormat sizedFormat);
std::size_t num_bytes(EGHIPixelComponent component);
std::size_t num_bytes(EGHIStorageElement element);

template<CEnum DstEnumType>
DstEnumType translate_to(EPicturePixelComponent pictureComponent);

}// end namespace ph::editor

#include "RenderCore/ghi_enums.ipp"
