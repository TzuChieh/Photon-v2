#pragma once

#include "Render/TextureResource.h"
#include "RenderCore/ghi_fwd.h"

#include <Utility/Concurrent/TRelaxedAtomic.h>

#include <memory>
#include <optional>

namespace ph::editor::render
{

class DetailedTexture : public TextureResource
{
public:
	using Base = TextureResource;

public:
	explicit DetailedTexture(std::unique_ptr<TextureResource> resource);

	std::size_t getWidthPx() const override;
	std::size_t getHeightPx() const override;
	std::size_t numLayers() const override;
	GHITextureHandle getGHITextureHandle() const override;
	void setupGHI(GHIThreadCaller& caller) override;
	void cleanupGHI(GHIThreadCaller& caller) override;

	std::optional<GHITextureNativeHandle> tryGetNativeHandle() const;

private:
	std::unique_ptr<TextureResource> m_resource;
	TRelaxedAtomic<std::optional<GHITextureNativeHandle>> m_sharedNativeHandle;
};

}// end namespace ph::editor::render
