#pragma once

#include "RenderCore/ghi_fwd.h"
#include "RenderCore/ghi_enums.h"
#include "EditorCore/Storage/TWeakHandle.h"

#include <Common/primitive_type.h>
#include <Math/TVector2.h>

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
	EGHIPixelFormat pixelFormat = EGHIPixelFormat::Empty;
	GHIInfoSampleState sampleState;

	/*!
	@return `true` if the conversion is an exact match.
	*/
	bool toFramebufferFormat(GHIInfoFramebufferFormat& framebufferFormat) const;
};

class GHIInfoFramebufferFormat final
{
public:
	EGHIPixelFormat pixelFormat = EGHIPixelFormat::Empty;
	GHIInfoSampleState sampleState;

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

}// end namespace ph::editor
