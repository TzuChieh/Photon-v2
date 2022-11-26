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

}// end namespace ph::editor
