#pragma once

#include "RenderCore/RTRResource.h"
#include "RenderCore/GHITexture2D.h"

#include <Math/TVector2.h>

#include <memory>

namespace ph::editor
{

class RTRTexture2D : public RTRResource
{
public:
	RTRTexture2D(const math::Vector2UI& sizePx, const GHIInfoTextureFormat& format);

	void createGHIResource(GHIThreadCaller& caller) override;
	void destroyGHIResource(GHIThreadCaller& caller) override;

private:
	math::Vector2UI m_sizePx;
	GHIInfoTextureFormat m_format;
	std::shared_ptr<GHITexture2D> m_ghiTexture;
};

}// end namespace ph::editor
