#pragma once

#include "RenderCore/ghi_fwd.h"
#include "RenderCore/ghi_enums.h"

#include <Common/primitive_type.h>
#include <Math/TVector2.h>
#include <Math/TVector3.h>

#include <cstddef>
#include <array>

namespace ph::editor::ghi
{

class SampleStateInfo final
{
public:
	EFilterMode filterMode = EFilterMode::Linear;
	EWrapMode wrapMode = EWrapMode::Repeat;
};

class TextureFormatInfo final
{
public:
	SampleStateInfo sampleState;
	ESizedPixelFormat pixelFormat = ESizedPixelFormat::Empty;
	uint8 numSamples : 4 = 1;
};

class FramebufferFormatInfo final
{
public:
	ESizedPixelFormat pixelFormat = ESizedPixelFormat::Empty;

	bool isEmpty() const
	{
		return pixelFormat == ESizedPixelFormat::Empty;
	}
};

/*!
Basically stores a shader reference for each `EGHIShadingStage` entry.
*/
class ShaderSetInfo final
{
public:
	ShaderHandle vertexShader;
	ShaderHandle fragmentShader;
	ShaderHandle computeShader;
};

class VertexAttributeLocatorInfo final
{
public:
	/*! Number of bytes to offset from the start of the vertex data. */
	std::size_t strideOffset = 0;

	/*! Number of bytes to step over to reach the next attribute data. Effectively added to `strideOffset` 
	i times for the i-th attribute.
	*/
	uint16 strideSize : 10 = 0;

	uint16 numElements : 2 = 0;
	uint16 shouldNormalize : 1 = false;

	EStorageElement elementType = EStorageElement::Empty;

	/*! @brief Empty attribute.
	*/
	inline VertexAttributeLocatorInfo() = default;

	bool isEmpty() const;
	std::size_t numAttributeBytes() const;
};

class VertexGroupFormatInfo final
{
public:
	inline constexpr static uint8 MAX_ATTRIBUTES = 8;

	std::array<VertexAttributeLocatorInfo, MAX_ATTRIBUTES> attributes;

	std::size_t numGroupBytes() const;
};

class DeviceCapabilityInfo final
{
public:
	uint8 maxTextureUnitsForVertexShadingStage : 6 = 0;
	uint8 maxTextureUnitsForFragmentShadingStage : 6 = 0;

	/*!  
	Note that the maximum number of elements/components of a single attribute is generally 4.
	A 4x4 matrix would normally occupy 4 attributes.
	*/
	uint8 maxVertexAttributes : 5 = 0;
};

class TextureDesc final
{
public:
	math::Vector3UI sizePx = {0, 0, 0};
	TextureFormatInfo format;

	TextureDesc& setSize1D(const uint32 lengthPx)
	{
		sizePx.x() = lengthPx;
		sizePx.y() = 1;
		sizePx.z() = 1;
		return *this;
	}

	TextureDesc& setSize2D(const math::Vector2UI& widthAndHeightPx)
	{
		sizePx.x() = widthAndHeightPx.x();
		sizePx.y() = widthAndHeightPx.y();
		sizePx.z() = 1;
		return *this;
	}
};

class FramebufferDesc final
{
public:
	inline constexpr static uint8 MAX_COLOR_ATTACHMENTS = 8;

public:
	math::Vector2UI sizePx = {0, 0};
	std::array<FramebufferFormatInfo, MAX_COLOR_ATTACHMENTS> colorFormats;
	FramebufferFormatInfo depthStencilFormat;
	uint8 numSamples : 4 = 1;
};

}// end namespace ph::editor::ghi
