#include "RenderCore/GHIFramebuffer.h"
#include "RenderCore/GHITexture.h"

namespace ph::editor
{

GHIInfoFramebufferFormat::GHIInfoFramebufferFormat()
	: pixelFormat(EGHIInfoPixelFormat::Empty)
	, sampleState()
{}

bool GHIInfoFramebufferFormat::toTextureFormat(GHIInfoTextureFormat& out_textureFormat) const
{
	out_textureFormat.pixelFormat = pixelFormat;
	out_textureFormat.sampleState = sampleState;
	return true;
}

GHIInfoFramebufferAttachment::GHIInfoFramebufferAttachment()
	: sizePx(0, 0)
	, colorFormats()
	, depthStencilFormat()
	, numSamples(1)
{}

GHIFramebuffer::GHIFramebuffer(const GHIInfoFramebufferAttachment& attachments)
	: m_attachments(attachments)
{}

GHIFramebuffer::~GHIFramebuffer() = default;

std::shared_ptr<GHITexture2D> GHIFramebuffer::createTextureFromColor(const uint32 /* slotIndex */)
{
	return nullptr;
}

std::shared_ptr<GHITexture2D> GHIFramebuffer::createTextureFromDepthStencil()
{
	return nullptr;
}

std::shared_ptr<GHIFramebuffer> GHIFramebuffer::getSharedPtrFromThis()
{
	return shared_from_this();
}

}// end namespace ph::editor
