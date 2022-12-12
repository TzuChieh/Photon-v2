#pragma once

#include "Render/RTRResource.h"
#include "RenderCore/GHIFramebuffer.h"

#include <Math/TVector2.h>

#include <memory>

namespace ph { class PictureData; }

namespace ph::editor
{

class RTRFramebufferResource : public RTRResource
{
public:
	explicit RTRFramebufferResource(const GHIInfoFramebufferAttachment& attachments);
	~RTRFramebufferResource();

	void setupGHI(GHIThreadCaller& caller) override;
	void cleanupGHI(GHIThreadCaller& caller) override;

	const GHIInfoFramebufferAttachment& getAttachments() const;
	const std::shared_ptr<GHIFramebuffer>& getGHIFramebuffer() const;

private:
	GHIInfoFramebufferAttachment m_attachments;
	std::shared_ptr<GHIFramebuffer> m_ghiFramebuffer;
};

inline const GHIInfoFramebufferAttachment& RTRFramebufferResource::getAttachments() const
{
	return m_attachments;
}

inline const std::shared_ptr<GHIFramebuffer>& RTRFramebufferResource::getGHIFramebuffer() const
{
	return m_ghiFramebuffer;
}

}// end namespace ph::editor
