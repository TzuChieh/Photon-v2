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

	const GHIInfoFramebufferAttachment& getAttachments() const;

	void setupGHI(GHIThreadCaller& caller) override;
	void cleanupGHI(GHIThreadCaller& caller) override;

	GHIFramebuffer* getGHIFramebuffer() const;
	std::shared_ptr<GHIFramebuffer> getGHIFramebufferResource() const;

private:
	GHIInfoFramebufferAttachment m_attachments;

	std::shared_ptr<GHIFramebuffer> m_ghiFramebuffer;
};

inline const GHIInfoFramebufferAttachment& RTRFramebufferResource::getAttachments() const
{
	return m_attachments;
}

inline GHIFramebuffer* RTRFramebufferResource::getGHIFramebuffer() const
{
	return m_ghiFramebuffer.get();
}

inline std::shared_ptr<GHIFramebuffer> RTRFramebufferResource::getGHIFramebufferResource() const
{
	return m_ghiFramebuffer;
}

}// end namespace ph::editor
