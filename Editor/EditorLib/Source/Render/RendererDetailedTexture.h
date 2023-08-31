#pragma once

#include "Render/RendererTexture.h"
#include "RenderCore/ghi_fwd.h"

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

	std::size_t getWidthPx() const override;
	std::size_t getHeightPx() const override;
	std::size_t numLayers() const override;
	GHITextureHandle getGHITextureHandle() const override;
	void setupGHI(GHIThreadCaller& caller) override;
	void cleanupGHI(GHIThreadCaller& caller) override;

	std::optional<GHITextureNativeHandle> tryGetNativeHandle() const;

private:
	std::unique_ptr<RendererTexture> m_resource;
	TRelaxedAtomic<std::optional<GHITextureNativeHandle>> m_sharedNativeHandle;
};

}// end namespace ph::editor
