#pragma once

#include "RenderCore/ghi_infos.h"

#include <Common/primitive_type.h>
#include <Math/TVector4.h>

#include <cstddef>
#include <memory>

namespace ph::editor
{

class GHITexture2D;

class GHIFramebuffer : public std::enable_shared_from_this<GHIFramebuffer>
{
public:
	explicit GHIFramebuffer(const GHIInfoFramebufferAttachment& attachments);
	virtual ~GHIFramebuffer();

	virtual void bind() = 0;
	virtual void unbind() = 0;
	virtual void clearColor(uint32 attachmentIndex, const math::Vector4F& color) = 0;
	virtual void clearDepthStencil(float32 depth, uint8 stencil) = 0;

	virtual std::shared_ptr<GHITexture2D> createTextureFromColor(uint32 attachmentIndex);
	virtual std::shared_ptr<GHITexture2D> createTextureFromDepthStencil();

	std::shared_ptr<GHIFramebuffer> getSharedPtrFromThis();
	const GHIInfoFramebufferAttachment& getAttachments() const;

private:
	GHIInfoFramebufferAttachment m_attachments;
};

inline const GHIInfoFramebufferAttachment& GHIFramebuffer::getAttachments() const
{
	return m_attachments;
}

}// end namespace ph::editor
