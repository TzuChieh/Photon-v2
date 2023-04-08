#pragma once

#include "Render/RendererTexture.h"
#include "RenderCore/GHITexture2D.h"

#include <Math/TVector2.h>
#include <Utility/utility.h>

#include <memory>

namespace ph { class PictureData; }

namespace ph::editor
{

class RendererTexture2D : public RendererTexture
{
public:
	RendererTexture2D(
		const GHIInfoTextureFormat& format, 
		std::unique_ptr<PictureData> textureData);

	~RendererTexture2D() override;

	std::size_t getWidthPx() const override;
	std::size_t getHeightPx() const override;
	const GHIInfoTextureFormat& getFormat() const;

	GHITexture* getGHITexture() const override;
	std::shared_ptr<GHITexture> getGHITextureResource() const override;
	void setupGHI(GHIThreadCaller& caller) override;
	void cleanupGHI(GHIThreadCaller& caller) override;

	std::shared_ptr<GHITexture2D> getGHITexture2DResource() const;

private:
	math::Vector2UI m_sizePx;
	GHIInfoTextureFormat m_format;

	std::shared_ptr<GHITexture2D> m_ghiTexture;
	std::unique_ptr<PictureData> m_textureData;
};

inline std::size_t RendererTexture2D::getWidthPx() const
{
	return lossless_integer_cast<std::size_t>(m_sizePx.x());
}

inline std::size_t RendererTexture2D::getHeightPx() const
{
	return lossless_integer_cast<std::size_t>(m_sizePx.y());
}

inline const GHIInfoTextureFormat& RendererTexture2D::getFormat() const
{
	return m_format;
}

inline GHITexture* RendererTexture2D::getGHITexture() const
{
	return m_ghiTexture.get();
}

inline std::shared_ptr<GHITexture> RendererTexture2D::getGHITextureResource() const
{
	return getGHITexture2DResource();
}

inline std::shared_ptr<GHITexture2D> RendererTexture2D::getGHITexture2DResource() const
{
	return m_ghiTexture;
}

}// end namespace ph::editor
