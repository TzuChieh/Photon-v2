#pragma once

#include "RenderCore/EGraphicsAPI.h"
#include "RenderCore/EClearTarget.h"

#include <Frame/picture_basics.h>
#include <Utility/traits.h>

#include <cstddef>

namespace ph::editor::ghi
{

/*! @brief Combined pixel layout and component type information.
Unless stated explicitly, the characters RGB does not mean the pixel is in RGB color space. 
It simply represents pixel components, using R, G, B as placeholders.
*/
enum class ESizedPixelFormat : uint8
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

enum class EPixelFormat : uint8
{
	Empty = 0,
	R,
	RG,
	RGB,
	RGBA,
	Depth,
	Stencil
};

enum class EPixelComponent : uint8
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

enum class EFilterMode : uint8
{
	Point,
	Linear
};

enum class EWrapMode : uint8
{
	ClampToEdge,
	Repeat
};

enum class EStorageElement : uint8
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

enum class EStorageUsage : uint8
{
	Unspecified = 0,
	Static,
	Dynamic
};

enum class EMeshDrawMode : uint8
{
	Points,
	LineSegments,
	LineCurveOpened,
	LineCurveClosed,
	TriangleStrip,
	TriangleFan,
	Triangles
};

enum class EShadingStage : uint8
{
	Unspecified = 0,
	Vertex,
	Fragment,
	Compute
};

std::size_t num_bytes(ESizedPixelFormat sizedFormat);
std::size_t num_bytes(EPixelComponent component);
std::size_t num_bytes(EStorageElement element);

template<CEnum DstEnumType>
DstEnumType translate_to(EPicturePixelComponent pictureComponent);

}// end namespace ph::editor::ghi

#include "RenderCore/ghi_enums.ipp"
