#pragma once

#include "Render/RTRTexture2DResource.h"
#include "RenderCore/GHITexture2D.h"

#include <optional>
#include <atomic>

namespace ph::editor
{

class RTRDetailedTexture2DResource : public RTRTexture2DResource
{
public:
	using Base = RTRTexture2DResource;

public:
	using Base::Base;

	void setupGHI(GHIThreadCaller& caller) override;
	void cleanupGHI(GHIThreadCaller& caller) override;

	std::optional<GHITexture2D::NativeHandle> tryGetNativeHandle();

private:
	std::atomic_flag           m_hasNativeHandleWritten;
	GHITexture2D::NativeHandle m_sharedNativeHandle;
};

}// end namespace ph::editor
