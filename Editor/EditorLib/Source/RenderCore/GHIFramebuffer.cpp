#include "RenderCore/GHIFramebuffer.h"
#include "RenderCore/GHITexture.h"

namespace ph::editor
{

GHIInfoFramebufferFormat::GHIInfoFramebufferFormat()
	: pixelFormat(EGHIInfoPixelFormat::Empty)
	, sampleState()
{}

GHIInfoTextureFormat GHIInfoFramebufferFormat::toTextureFormat() const
{
	GHIInfoTextureFormat textureFormat;
	textureFormat.pixelFormat = pixelFormat;
	textureFormat.sampleState = sampleState;
	return textureFormat;
}

GHIInfoFramebufferAttachment::GHIInfoFramebufferAttachment()
	: sizePx(0, 0)
	, numSamples(1)
	, colorFormats()
	, depthStencilFormat()
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
