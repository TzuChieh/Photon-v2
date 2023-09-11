#pragma once

#include "Render/TextureResource.h"
#include "RenderCore/ghi_infos.h"

#include <Math/TVector2.h>
#include <Utility/utility.h>

#include <memory>

namespace ph { class PictureData; }

namespace ph::editor::render
{

class Texture2D : public TextureResource
{
public:
	Texture2D(
		const GHIInfoTextureFormat& format, 
		std::unique_ptr<PictureData> textureData);

	~Texture2D() override;

	std::size_t getWidthPx() const override;
	std::size_t getHeightPx() const override;
	GHITextureHandle getGHITextureHandle() const override;
	const GHIInfoTextureFormat& getFormat() const;

	void setupGHI(GHIThreadCaller& caller) override;
	void cleanupGHI(GHIThreadCaller& caller) override;

private:
	math::Vector2UI m_sizePx;
	GHIInfoTextureFormat m_format;

	GHITextureHandle m_textureHandle;
	std::unique_ptr<PictureData> m_textureData;
};

inline std::size_t Texture2D::getWidthPx() const
{
	return lossless_integer_cast<std::size_t>(m_sizePx.x());
}

inline std::size_t Texture2D::getHeightPx() const
{
	return lossless_integer_cast<std::size_t>(m_sizePx.y());
}

inline GHITextureHandle Texture2D::getGHITextureHandle() const
{
	return m_textureHandle;
}

inline const GHIInfoTextureFormat& Texture2D::getFormat() const
{
	return m_format;
}

}// end namespace ph::editor::render
