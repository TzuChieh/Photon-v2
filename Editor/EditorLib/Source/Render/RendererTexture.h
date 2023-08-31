#pragma once

#include "Render/RendererResource.h"
#include "RenderCore/ghi_fwd.h"

#include <memory>
#include <cstddef>

namespace ph::editor
{

class RendererTexture : public RendererResource
{
public:
	RendererTexture();

	~RendererTexture() override;

	virtual std::size_t getWidthPx() const;
	virtual std::size_t getHeightPx() const;
	virtual std::size_t numLayers() const;
	virtual GHITextureHandle getGHITextureHandle() const = 0;

	void setupGHI(GHIThreadCaller& caller) override = 0;
	void cleanupGHI(GHIThreadCaller& caller) override = 0;
};

inline std::size_t RendererTexture::getWidthPx() const
{
	return 0;
}

inline std::size_t RendererTexture::getHeightPx() const
{
	return 0;
}

inline std::size_t RendererTexture::numLayers() const
{
	return 0;
}

}// end namespace ph::editor
