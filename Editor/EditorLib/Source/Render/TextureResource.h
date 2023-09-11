#pragma once

#include "Render/SceneResource.h"
#include "RenderCore/ghi_fwd.h"

#include <memory>
#include <cstddef>

namespace ph::editor::render
{

class TextureResource : public SceneResource
{
public:
	TextureResource();

	~TextureResource() override;

	virtual std::size_t getWidthPx() const;
	virtual std::size_t getHeightPx() const;
	virtual std::size_t numLayers() const;
	virtual GHITextureHandle getGHITextureHandle() const = 0;

	void setupGHI(GHIThreadCaller& caller) override = 0;
	void cleanupGHI(GHIThreadCaller& caller) override = 0;
};

inline std::size_t TextureResource::getWidthPx() const
{
	return 0;
}

inline std::size_t TextureResource::getHeightPx() const
{
	return 0;
}

inline std::size_t TextureResource::numLayers() const
{
	return 0;
}

}// end namespace ph::editor::render
