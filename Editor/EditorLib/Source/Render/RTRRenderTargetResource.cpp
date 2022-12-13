#include "Render/RTRRenderTargetResource.h"
#include "Render/RTRFramebufferResource.h"

#include <Common/assertion.h>

namespace ph::editor
{

RTRRenderTargetResource::RTRRenderTargetResource(
	const GHIInfoTextureFormat& format,
	const math::Vector2S& sizePx)

	: RTRTextureResource()

	, m_sizePx(safe_integer_cast<uint32f>(sizePx.x()), safe_integer_cast<uint32f>(sizePx.y()), 0)
	, m_format(format)
	, m_attachmentIndex(0)
	, m_isDepthStencilAttachment(false)

	, m_ghiTexture(nullptr)
	, m_ghiFramebuffer(nullptr)
	, m_framebufferResource(nullptr)
{}
	
RTRRenderTargetResource::RTRRenderTargetResource(
	RTRFramebufferResource* const framebufferResource,
	const uint32 attachmentIndex,
	const bool isDepthStencilAttachment)

	: RTRTextureResource()

	, m_sizePx(0)
	, m_format()
	, m_attachmentIndex(attachmentIndex)
	, m_isDepthStencilAttachment(isDepthStencilAttachment)

	, m_ghiTexture(nullptr)
	, m_ghiFramebuffer(nullptr)
	, m_framebufferResource(framebufferResource)
{
	PH_ASSERT(framebufferResource);

	if(!isDepthStencilAttachment)
	{
		PH_ASSERT_LT(attachmentIndex, framebufferResource->getAttachments().colorFormats.size());

		const GHIInfoFramebufferFormat& fbFormat = framebufferResource->getAttachments().colorFormats[m_attachmentIndex];
		m_format = fbFormat.toTextureFormat();
	}
	else
	{
		const GHIInfoFramebufferFormat& fbFormat = framebufferResource->getAttachments().depthStencilFormat;
		m_format = fbFormat.toTextureFormat();
	}
}

void RTRRenderTargetResource::setupGHI(GHIThreadCaller& caller)
{
	// TODO
}

void RTRRenderTargetResource::cleanupGHI(GHIThreadCaller& caller)
{
	// TODO
}

}// end namespace ph::editor
