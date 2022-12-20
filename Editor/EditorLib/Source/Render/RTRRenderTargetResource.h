#pragma once

#include "Render/RTRTextureResource.h"
#include "RenderCore/GHITexture.h"
#include "RenderCore/GHIFramebuffer.h"

#include <Common/primitive_type.h>
#include <Math/TVector3.h>
#include <Utility/utility.h>

#include <memory>

namespace ph { class PictureData; }

namespace ph::editor
{

class RTRFramebufferResource;

class RTRRenderTargetResource : public RTRTextureResource
{
public:
	/*! @brief A texture render target with custom format.
	*/
	RTRRenderTargetResource(
		const GHIInfoTextureFormat& format,
		const math::Vector2S& sizePx);

	/*! @brief A texture render target backed by a framebuffer.
	*/
	RTRRenderTargetResource(
		RTRFramebufferResource* framebufferResource,
		uint32 attachmentIndex,
		bool isDepthStencilAttachment = false);

	~RTRRenderTargetResource() override;

	std::size_t getWidthPx() const override;
	std::size_t getHeightPx() const override;
	std::size_t numLayers() const override;
	const GHIInfoTextureFormat& getFormat() const;

	GHITexture* getGHITexture() const override;
	std::shared_ptr<GHITexture> getGHITextureResource() const override;
	void setupGHI(GHIThreadCaller& caller) override;
	void cleanupGHI(GHIThreadCaller& caller) override;

	GHIFramebuffer* getGHIFramebuffer() const;
	std::shared_ptr<GHIFramebuffer> getGHIFramebufferResource() const;

private:
	math::Vector3UI m_sizePx;
	GHIInfoTextureFormat m_format;
	uint32 m_attachmentIndex;
	bool m_isDepthStencilAttachment;

	std::shared_ptr<GHITexture> m_ghiTexture;
	std::shared_ptr<GHIFramebuffer> m_ghiFramebuffer;
	RTRFramebufferResource* m_framebufferResource;
};

inline std::size_t RTRRenderTargetResource::getWidthPx() const
{
	return lossless_integer_cast<std::size_t>(m_sizePx.x());
}

inline std::size_t RTRRenderTargetResource::getHeightPx() const
{
	return lossless_integer_cast<std::size_t>(m_sizePx.y());
}

inline std::size_t RTRRenderTargetResource::numLayers() const
{
	return lossless_integer_cast<std::size_t>(m_sizePx.z());
}

inline const GHIInfoTextureFormat& RTRRenderTargetResource::getFormat() const
{
	return m_format;
}

inline GHITexture* RTRRenderTargetResource::getGHITexture() const
{
	return m_ghiTexture.get();
}

inline std::shared_ptr<GHITexture> RTRRenderTargetResource::getGHITextureResource() const
{
	return m_ghiTexture;
}

inline GHIFramebuffer* RTRRenderTargetResource::getGHIFramebuffer() const
{
	return m_ghiFramebuffer.get();
}

inline std::shared_ptr<GHIFramebuffer> RTRRenderTargetResource::getGHIFramebufferResource() const
{
	return m_ghiFramebuffer;
}

}// end namespace ph::editor
