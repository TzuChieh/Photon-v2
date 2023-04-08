#pragma once

#include "Render/RendererResource.h"
#include "RenderCore/GHIFramebuffer.h"

#include <Math/TVector2.h>

#include <memory>

namespace ph { class PictureData; }

namespace ph::editor
{

class RendererFramebuffer : public RendererResource
{
public:
	explicit RendererFramebuffer(const GHIInfoFramebufferAttachment& attachments);
	~RendererFramebuffer() override;

	const GHIInfoFramebufferAttachment& getAttachments() const;

	void setupGHI(GHIThreadCaller& caller) override;
	void cleanupGHI(GHIThreadCaller& caller) override;

	GHIFramebuffer* getGHIFramebuffer() const;
	std::shared_ptr<GHIFramebuffer> getGHIFramebufferResource() const;

private:
	GHIInfoFramebufferAttachment m_attachments;

	std::shared_ptr<GHIFramebuffer> m_ghiFramebuffer;
};

inline const GHIInfoFramebufferAttachment& RendererFramebuffer::getAttachments() const
{
	return m_attachments;
}

inline GHIFramebuffer* RendererFramebuffer::getGHIFramebuffer() const
{
	return m_ghiFramebuffer.get();
}

inline std::shared_ptr<GHIFramebuffer> RendererFramebuffer::getGHIFramebufferResource() const
{
	return m_ghiFramebuffer;
}

}// end namespace ph::editor
