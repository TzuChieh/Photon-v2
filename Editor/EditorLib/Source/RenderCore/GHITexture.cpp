#include "RenderCore/GHITexture.h"
#include "RenderCore/GHIFramebuffer.h"

namespace ph::editor
{

GHITexture::~GHITexture() = default;

GHITexture::GHITexture(const GHIInfoTextureFormat& format)
	: m_format(format)
{}

void GHITexture::setFormat(const GHIInfoTextureFormat& format)
{
	m_format = format;
}

}// end namespace ph::editor
