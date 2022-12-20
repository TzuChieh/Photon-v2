#include "Render/RTRRenderTargetResource.h"
#include "Render/RTRFramebufferResource.h"
#include "RenderCore/GHIThreadCaller.h"
#include "RenderCore/GHITexture2D.h"

#include <Common/assertion.h>
#include <Common/logging.h>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(RenderTarget, RTRResource);

RTRRenderTargetResource::RTRRenderTargetResource(
	const GHIInfoTextureFormat& format,
	const math::Vector2S& sizePx)

	: RTRTextureResource()

	, m_sizePx(lossless_integer_cast<uint32f>(sizePx.x()), lossless_integer_cast<uint32f>(sizePx.y()), 0)
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
	const GHIInfoFramebufferAttachment& attachments = framebufferResource->getAttachments();

	m_sizePx.x() = attachments.sizePx.x();
	m_sizePx.y() = attachments.sizePx.y();

	GHIInfoFramebufferFormat framebufferFormat;
	if(!isDepthStencilAttachment)
	{
		PH_ASSERT_LT(attachmentIndex, attachments.colorFormats.size());
		framebufferFormat = attachments.colorFormats[m_attachmentIndex];
	}
	else
	{
		framebufferFormat = attachments.depthStencilFormat;
	}

	if(!framebufferFormat.toTextureFormat(m_format))
	{
		PH_LOG_WARNING(RenderTarget,
			"framebuffer format cannot be converted to texture format exactly");
	}

	if(attachments.numSamples > 1)
	{
		// Currently we do not support MSAA, may require an additional resolve process
		PH_LOG_ERROR(RenderTarget,
			"MSAA framebuffer is not supported (sample count {})", attachments.numSamples);
	}
}

RTRRenderTargetResource::~RTRRenderTargetResource()
{
	// Must have been released by GHI thread
	PH_ASSERT(!m_ghiTexture);
	PH_ASSERT(!m_ghiFramebuffer);
}

void RTRRenderTargetResource::setupGHI(GHIThreadCaller& caller)
{
	// Render target backed by existing framebuffer resource, copy the underlying GHI for our use
	if(m_framebufferResource)
	{
		const GHIInfoFramebufferAttachment& attachments = m_framebufferResource->getAttachments();

		caller.add(
			[this](GHI& ghi)
			{
				m_ghiFramebuffer = m_framebufferResource->getGHIFramebufferResource();

				GHIFramebuffer* const ghiFramebuffer = m_framebufferResource->getGHIFramebuffer();
				if(!m_isDepthStencilAttachment)
				{
					m_ghiTexture = ghiFramebuffer->createTextureFromColor(m_attachmentIndex);
				}
				else
				{
					m_ghiTexture = ghiFramebuffer->createTextureFromDepthStencil();
				}

				// Indicate we are done using it
				m_framebufferResource = nullptr;
			});
	}
	// Create our own GHI resource
	else
	{
		// TODO
	}
}

void RTRRenderTargetResource::cleanupGHI(GHIThreadCaller& caller)
{
	caller.add(
		[this](GHI& ghi)
		{
			m_ghiTexture = nullptr;
			m_ghiFramebuffer = nullptr;
		});
}

}// end namespace ph::editor
