#pragma once

#include "Render/RendererTexture.h"
#include "RenderCore/GHITexture.h"

#include <Utility/Concurrent/TRelaxedAtomic.h>

#include <memory>
#include <optional>

namespace ph::editor
{

class RendererDetailedTexture : public RendererTexture
{
public:
	using Base = RendererTexture;

public:
	explicit RendererDetailedTexture(std::unique_ptr<RendererTexture> resource);

	GHITexture* getGHITexture() const override;
	std::shared_ptr<GHITexture> getGHITextureResource() const override;
	std::size_t getWidthPx() const override;
	std::size_t getHeightPx() const override;
	std::size_t numLayers() const override;
	void setupGHI(GHIThreadCaller& caller) override;
	void cleanupGHI(GHIThreadCaller& caller) override;

	std::optional<GHITexture::NativeHandle> tryGetNativeHandle() const;

private:
	std::unique_ptr<RendererTexture> m_resource;
	TRelaxedAtomic<std::optional<GHITexture::NativeHandle>> m_sharedNativeHandle;
};

}// end namespace ph::editor
