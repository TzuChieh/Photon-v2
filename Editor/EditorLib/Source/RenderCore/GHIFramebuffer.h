#pragma once

#include "RenderCore/ghi_enums.h"
#include "RenderCore/ghi_states.h"

#include <Common/primitive_type.h>
#include <Math/TVector2.h>
#include <Math/TVector4.h>

#include <cstddef>
#include <array>

namespace ph::editor
{

class GHIFramebufferFormat final
{
public:
	EGHIPixelFormat pixelFormat;
	GHISampleState sampleState;

	GHIFramebufferFormat();
};

class GHIFramebufferAttachmentInfo final
{
public:
	inline constexpr static uint32 MAX_COLOR_ATTACHMENTS = 8;

public:
	math::TVector2<uint32> sizePx;
	uint32 numSamples;
	std::array<GHIFramebufferFormat, MAX_COLOR_ATTACHMENTS> colorFormats;
	GHIFramebufferFormat depthStencilFormat;

	GHIFramebufferAttachmentInfo();
};

class GHIFramebuffer
{
public:
	virtual ~GHIFramebuffer();

	virtual void bind() = 0;
	virtual void unbind() = 0;
	virtual void setAttachments(const GHIFramebufferAttachmentInfo& attachments) = 0;
	virtual void clearColor(uint32 slotIndex, const math::Vector4F& color) = 0;
	virtual void clearDepthStencil(float32 depth, uint8 stencil) = 0;
};

}// end namespace ph::editor
