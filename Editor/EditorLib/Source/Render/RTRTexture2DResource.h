#pragma once

#include "Render/RTRResource.h"
#include "RenderCore/GHITexture2D.h"

#include <Math/TVector2.h>

#include <memory>

namespace ph::editor
{

class RTRTexture2DResource : public RTRResource
{
public:
	RTRTexture2DResource(const math::Vector2UI& sizePx, const GHIInfoTextureFormat& format);

	void setupGHI(GHIThreadCaller& caller) override;
	void cleanupGHI(GHIThreadCaller& caller) override;

	const math::Vector2UI& getSizePx() const;
	const GHIInfoTextureFormat& getFormat() const;
	const std::shared_ptr<GHITexture2D>& getGHITexture() const;

private:
	math::Vector2UI m_sizePx;
	GHIInfoTextureFormat m_format;
	std::shared_ptr<GHITexture2D> m_ghiTexture;
};

inline const math::Vector2UI& RTRTexture2DResource::getSizePx() const
{
	return m_sizePx;
}

inline const GHIInfoTextureFormat& RTRTexture2DResource::getFormat() const
{
	return m_format;
}

inline const std::shared_ptr<GHITexture2D>& RTRTexture2DResource::getGHITexture() const
{
	return m_ghiTexture;
}

}// end namespace ph::editor
