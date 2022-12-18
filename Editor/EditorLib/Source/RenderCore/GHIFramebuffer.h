#pragma once

#include "RenderCore/ghi_enums.h"
#include "RenderCore/ghi_states.h"

#include <Common/primitive_type.h>
#include <Math/TVector2.h>
#include <Math/TVector4.h>

#include <cstddef>
#include <array>
#include <memory>

namespace ph::editor
{

class GHITexture2D;
class GHIInfoTextureFormat;

class GHIInfoFramebufferFormat final
{
public:
	EGHIInfoPixelFormat pixelFormat;
	GHIInfoSampleState sampleState;

	GHIInfoFramebufferFormat();

	/*!
	@return `true` if the conversion is an exact match.
	*/
	bool toTextureFormat(GHIInfoTextureFormat& textureFormat) const;
};

class GHIInfoFramebufferAttachment final
{
public:
	inline constexpr static uint8 MAX_COLOR_ATTACHMENTS = 8;

public:
	math::TVector2<uint32> sizePx;
	std::array<GHIInfoFramebufferFormat, MAX_COLOR_ATTACHMENTS> colorFormats;
	GHIInfoFramebufferFormat depthStencilFormat;
	uint8 numSamples;

	GHIInfoFramebufferAttachment();
};

class GHIFramebuffer : public std::enable_shared_from_this<GHIFramebuffer>
{
public:
	virtual ~GHIFramebuffer();

	virtual void bind() = 0;
	virtual void unbind() = 0;
	virtual void setAttachments(const GHIInfoFramebufferAttachment& attachments) = 0;
	virtual void clearColor(uint32 attachmentIndex, const math::Vector4F& color) = 0;
	virtual void clearDepthStencil(float32 depth, uint8 stencil) = 0;

	virtual std::shared_ptr<GHITexture2D> createTextureFromColor(uint32 attachmentIndex);
	virtual std::shared_ptr<GHITexture2D> createTextureFromDepthStencil();

	std::shared_ptr<GHIFramebuffer> getSharedPtrFromThis();
};

}// end namespace ph::editor
