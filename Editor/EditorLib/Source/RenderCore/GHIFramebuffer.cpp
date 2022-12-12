#include "RenderCore/GHIFramebuffer.h"

namespace ph::editor
{

GHIInfoFramebufferFormat::GHIInfoFramebufferFormat()
	: pixelFormat(EGHIInfoPixelFormat::Empty)
	, sampleState()
{}

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

}// end namespace ph::editor
