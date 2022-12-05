#pragma once

#include "Render/RTRTexture2DResource.h"
#include "RenderCore/GHITexture2D.h"

#include <Utility/Concurrent/TRelaxedRW.h>

#include <optional>

namespace ph::editor
{

class RTRDetailedTexture2DResource : public RTRTexture2DResource
{
public:
	using Base = RTRTexture2DResource;

public:
	RTRDetailedTexture2DResource(
		const math::Vector2UI& sizePx, 
		const GHIInfoTextureFormat& format);

	void setupGHI(GHIThreadCaller& caller) override;
	void cleanupGHI(GHIThreadCaller& caller) override;

	std::optional<GHITexture2D::NativeHandle> tryGetNativeHandle() const;

private:
	TRelaxedRW<std::optional<GHITexture2D::NativeHandle>> m_sharedNativeHandle;
};

}// end namespace ph::editor
