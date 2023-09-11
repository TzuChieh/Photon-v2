#pragma once

#include "Render/TextureResource.h"
#include "RenderCore/GHIFramebuffer.h"

#include <Math/TVector2.h>

#include <memory>

namespace ph { class PictureData; }

namespace ph::editor::render
{

class Framebuffer : public TextureResource
{
public:
	explicit Framebuffer(const GHIInfoFramebufferAttachment& attachments);
	~Framebuffer() override;

	const GHIInfoFramebufferAttachment& getAttachments() const;

	void setupGHI(GHIThreadCaller& caller) override;
	void cleanupGHI(GHIThreadCaller& caller) override;

	GHIFramebuffer* getGHIFramebuffer() const;
	std::shared_ptr<GHIFramebuffer> getGHIFramebufferResource() const;

private:
	GHIInfoFramebufferAttachment m_attachments;

	std::shared_ptr<GHIFramebuffer> m_ghiFramebuffer;
};

inline const GHIInfoFramebufferAttachment& Framebuffer::getAttachments() const
{
	return m_attachments;
}

inline GHIFramebuffer* Framebuffer::getGHIFramebuffer() const
{
	return m_ghiFramebuffer.get();
}

inline std::shared_ptr<GHIFramebuffer> Framebuffer::getGHIFramebufferResource() const
{
	return m_ghiFramebuffer;
}

}// end namespace ph::editor::render
