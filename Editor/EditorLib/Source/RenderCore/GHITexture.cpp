#include "RenderCore/GHITexture.h"
#include "RenderCore/GHIFramebuffer.h"

namespace ph::editor
{

GHIInfoTextureFormat::GHIInfoTextureFormat()
	: pixelFormat(EGHIInfoPixelFormat::Empty)
	, sampleState()
{}

bool GHIInfoTextureFormat::toTextureFormat(GHIInfoFramebufferFormat& framebufferFormat) const
{
	framebufferFormat.pixelFormat = pixelFormat;
	framebufferFormat.sampleState = sampleState;
	return true;
}

GHITexture::~GHITexture() = default;

GHITexture::GHITexture()
{}

}// end namespace ph::editor
