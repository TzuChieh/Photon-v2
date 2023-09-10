#pragma once

#include "RenderCore/ghi_fwd.h"
#include "RenderCore/ghi_enums.h"

#include <Common/primitive_type.h>
#include <Math/TVector2.h>
#include <Math/TVector3.h>

#include <cstddef>
#include <array>

namespace ph::editor
{

class GHIInfoSampleState final
{
public:
	EGHIFilterMode filterMode = EGHIFilterMode::Linear;
	EGHIWrapMode wrapMode = EGHIWrapMode::Repeat;
};

class GHIInfoTextureFormat final
{
public:
	GHIInfoSampleState sampleState;
	EGHIPixelFormat pixelFormat = EGHIPixelFormat::Empty;

	/*!
	@return `true` if the conversion is an exact match.
	*/
	bool toFramebufferFormat(GHIInfoFramebufferFormat& framebufferFormat) const;
};

class GHIInfoFramebufferFormat final
{
public:
	GHIInfoSampleState sampleState;
	EGHIPixelFormat pixelFormat = EGHIPixelFormat::Empty;

	/*!
	@return `true` if the conversion is an exact match.
	*/
	bool toTextureFormat(GHIInfoTextureFormat& out_textureFormat) const;
};

class GHIInfoFramebufferAttachment final
{
public:
	inline constexpr static uint8 MAX_COLOR_ATTACHMENTS = 8;

public:
	math::Vector2UI sizePx = {0, 0};
	std::array<GHIInfoFramebufferFormat, MAX_COLOR_ATTACHMENTS> colorFormats;
	GHIInfoFramebufferFormat depthStencilFormat;
	uint8 numSamples = 1;
};

/*!
Basically stores a shader reference for each `EGHIShadingStage` entry.
*/
class GHIInfoShaderSet final
{
public:
	GHIShaderHandle vertexShader;
	GHIShaderHandle fragmentShader;
	GHIShaderHandle computeShader;
};

class GHIInfoVertexAttributeLocator final
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

	EGHIStorageElement elementType = EGHIStorageElement::Empty;

	/*! @brief Empty attribute.
	*/
	inline GHIInfoVertexAttributeLocator() = default;

	bool isEmpty() const;
	std::size_t numAttributeBytes() const;
};

class GHIInfoVertexGroupFormat final
{
public:
	inline constexpr static uint8 MAX_ATTRIBUTES = 8;

	std::array<GHIInfoVertexAttributeLocator, MAX_ATTRIBUTES> attributes;

	std::size_t numGroupBytes() const;
};

class GHIInfoDeviceCapability final
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

class GHIInfoTextureDesc final
{
public:
	math::Vector3UI sizePx = {0, 0, 0};
	GHIInfoTextureFormat format;

	inline GHIInfoTextureDesc& setSize1D(const uint32 lengthPx)
	{
		sizePx.x() = lengthPx;
		sizePx.y() = 1;
		sizePx.z() = 1;
		return *this;
	}

	inline GHIInfoTextureDesc& setSize2D(const math::Vector2UI& widthAndHeightPx)
	{
		sizePx.x() = widthAndHeightPx.x();
		sizePx.y() = widthAndHeightPx.y();
		sizePx.z() = 1;
		return *this;
	}
};

}// end namespace ph::editor
