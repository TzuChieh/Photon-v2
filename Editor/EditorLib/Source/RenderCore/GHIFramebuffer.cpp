#include "RenderCore/GHIFramebuffer.h"

namespace ph::editor
{

GHIFramebufferFormat::GHIFramebufferFormat()
	: pixelFormat(EGHIPixelFormat::Empty)
	, sampleState()
{}

GHIFramebufferAttachmentInfo::GHIFramebufferAttachmentInfo()
	: sizePx(0, 0)
	, numSamples(1)
	, colorFormats()
	, depthStencilFormat()
{}

GHIFramebuffer::~GHIFramebuffer() = default;

}// end namespace ph::editor
