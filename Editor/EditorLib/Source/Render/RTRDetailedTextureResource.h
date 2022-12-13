#pragma once

#include "Render/RTRTextureResource.h"
#include "RenderCore/GHITexture.h"

#include <Utility/Concurrent/TRelaxedRW.h>

#include <memory>
#include <optional>

namespace ph::editor
{

class RTRDetailedTextureResource : public RTRTextureResource
{
public:
	using Base = RTRTextureResource;

public:
	explicit RTRDetailedTextureResource(std::unique_ptr<RTRTextureResource> resource);

	GHITexture* getGHITexture() const override;
	std::shared_ptr<GHITexture> getGHITextureResource() const override;
	std::size_t getWidthPx() const override;
	std::size_t getHeightPx() const override;
	std::size_t numLayers() const override;
	void setupGHI(GHIThreadCaller& caller) override;
	void cleanupGHI(GHIThreadCaller& caller) override;

	std::optional<GHITexture::NativeHandle> tryGetNativeHandle() const;

private:
	std::unique_ptr<RTRTextureResource> m_resource;
	TRelaxedRW<std::optional<GHITexture::NativeHandle>> m_sharedNativeHandle;
};

}// end namespace ph::editor
