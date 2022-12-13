#pragma once

#include "Render/RTRResource.h"

#include <memory>
#include <cstddef>

namespace ph::editor
{

class GHITexture;

class RTRTextureResource : public RTRResource
{
public:
	RTRTextureResource();

	~RTRTextureResource() override;

	virtual GHITexture* getGHITexture() const = 0;
	virtual std::shared_ptr<GHITexture> getGHITextureResource() const = 0;
	void setupGHI(GHIThreadCaller& caller) override = 0;
	void cleanupGHI(GHIThreadCaller& caller) override = 0;

	virtual std::size_t getWidthPx() const;
	virtual std::size_t getHeightPx() const;
	virtual std::size_t numLayers() const;
};

inline std::size_t RTRTextureResource::getWidthPx() const
{
	return 0;
}

inline std::size_t RTRTextureResource::getHeightPx() const
{
	return 0;
}

inline std::size_t RTRTextureResource::numLayers() const
{
	return 0;
}

}// end namespace ph::editor
