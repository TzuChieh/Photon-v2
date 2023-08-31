#pragma once

#include "Render/RendererTexture.h"
#include "RenderCore/GHIFramebuffer.h"
#include "RenderCore/ghi_infos.h"

#include <Common/primitive_type.h>
#include <Math/TVector3.h>
#include <Utility/utility.h>

#include <memory>

namespace ph { class PictureData; }

namespace ph::editor
{

class RendererFramebuffer;

class RendererRenderTarget : public RendererTexture
{
public:
	/*! @brief A texture render target with custom format.
	*/
	RendererRenderTarget(
		const GHIInfoTextureFormat& format,
		const math::Vector2S& sizePx);

	/*! @brief A texture render target backed by a framebuffer.
	*/
	RendererRenderTarget(
		RendererFramebuffer* framebufferResource,
		uint32 attachmentIndex,
		bool isDepthStencilAttachment = false);

	~RendererRenderTarget() override;

	std::size_t getWidthPx() const override;
	std::size_t getHeightPx() const override;
	std::size_t numLayers() const override;
	GHITextureHandle getGHITextureHandle() const override;
	const GHIInfoTextureFormat& getFormat() const;

	void setupGHI(GHIThreadCaller& caller) override;
	void cleanupGHI(GHIThreadCaller& caller) override;

	GHIFramebuffer* getGHIFramebuffer() const;
	std::shared_ptr<GHIFramebuffer> getGHIFramebufferResource() const;

private:
	math::Vector3UI m_sizePx;
	GHIInfoTextureFormat m_format;
	uint32 m_attachmentIndex;
	bool m_isDepthStencilAttachment;

	GHITextureHandle m_textureHandle;
	std::shared_ptr<GHIFramebuffer> m_ghiFramebuffer;
	RendererFramebuffer* m_framebufferResource;
};

inline std::size_t RendererRenderTarget::getWidthPx() const
{
	return lossless_integer_cast<std::size_t>(m_sizePx.x());
}

inline std::size_t RendererRenderTarget::getHeightPx() const
{
	return lossless_integer_cast<std::size_t>(m_sizePx.y());
}

inline std::size_t RendererRenderTarget::numLayers() const
{
	return lossless_integer_cast<std::size_t>(m_sizePx.z());
}

inline const GHIInfoTextureFormat& RendererRenderTarget::getFormat() const
{
	return m_format;
}

inline GHITextureHandle RendererRenderTarget::getGHITextureHandle() const
{
	return m_textureHandle;
}

inline GHIFramebuffer* RendererRenderTarget::getGHIFramebuffer() const
{
	return m_ghiFramebuffer.get();
}

inline std::shared_ptr<GHIFramebuffer> RendererRenderTarget::getGHIFramebufferResource() const
{
	return m_ghiFramebuffer;
}

}// end namespace ph::editor
