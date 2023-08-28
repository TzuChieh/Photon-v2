#include "RenderCore/GHIFramebuffer.h"
#include "RenderCore/GHITexture.h"

namespace ph::editor
{

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
