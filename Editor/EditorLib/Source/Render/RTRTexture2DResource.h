#pragma once

#include "Render/RTRTextureResource.h"
#include "RenderCore/GHITexture2D.h"

#include <Math/TVector2.h>
#include <Utility/utility.h>

#include <memory>

namespace ph { class PictureData; }

namespace ph::editor
{

class RTRTexture2DResource : public RTRTextureResource
{
public:
	RTRTexture2DResource(
		const GHIInfoTextureFormat& format, 
		std::unique_ptr<PictureData> textureData);

	GHITexture* getGHITexture() const override;
	std::shared_ptr<GHITexture> getGHITextureResource() const override;
	void setupGHI(GHIThreadCaller& caller) override;
	void cleanupGHI(GHIThreadCaller& caller) override;

	std::size_t getWidthPx() const override;
	std::size_t getHeightPx() const override;
	const GHIInfoTextureFormat& getFormat() const;
	std::shared_ptr<GHITexture2D> getGHITexture2DResource() const;

private:
	math::Vector2UI m_sizePx;
	GHIInfoTextureFormat m_format;
	std::shared_ptr<GHITexture2D> m_ghiTexture;
	std::unique_ptr<PictureData> m_textureData;
};

inline std::size_t RTRTexture2DResource::getWidthPx() const
{
	return safe_integer_cast<std::size_t>(m_sizePx.x());
}

inline std::size_t RTRTexture2DResource::getHeightPx() const
{
	return safe_integer_cast<std::size_t>(m_sizePx.y());
}

inline const GHIInfoTextureFormat& RTRTexture2DResource::getFormat() const
{
	return m_format;
}

inline GHITexture* RTRTexture2DResource::getGHITexture() const
{
	return m_ghiTexture.get();
}

inline std::shared_ptr<GHITexture> RTRTexture2DResource::getGHITextureResource() const
{
	return getGHITexture2DResource();
}

inline std::shared_ptr<GHITexture2D> RTRTexture2DResource::getGHITexture2DResource() const
{
	return m_ghiTexture;
}

}// end namespace ph::editor
