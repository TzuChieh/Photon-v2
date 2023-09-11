#include "Render/DetailedTexture.h"
#include "RenderCore/ghi_infos.h"
#include "RenderCore/GHIThreadCaller.h"
#include "RenderCore/GraphicsContext.h"
#include "RenderCore/GHI.h"

#include <Frame/PictureData.h>

#include <utility>

namespace ph::editor::render
{

DetailedTexture::DetailedTexture(
	std::unique_ptr<TextureResource> resource)

	: TextureResource()

	, m_resource(std::move(resource))
	, m_sharedNativeHandle()
{}

void DetailedTexture::setupGHI(GHIThreadCaller& caller)
{
	if(m_resource)
	{
		m_resource->setupGHI(caller);
	}

	caller.add(
		[this](GraphicsContext& ctx)
		{
			GHITextureHandle handle = getGHITextureHandle();

			// Generally this should be true since the call is issued after base resource GHI 
			// creation (unless base resource is empty)
			if(handle)
			{
				GHITextureNativeHandle nativeHandle = ctx.getGHI().tryGetTextureNativeHandle(handle);
				m_sharedNativeHandle.relaxedWrite(nativeHandle);
			}
		});
}

void DetailedTexture::cleanupGHI(GHIThreadCaller& caller)
{
	caller.add(
		[this](GraphicsContext& /* ctx */)
		{
			m_sharedNativeHandle.relaxedWrite(GHITextureNativeHandle{});
		});

	if(m_resource)
	{
		m_resource->cleanupGHI(caller);
	}
}

std::optional<GHITextureNativeHandle> DetailedTexture::tryGetNativeHandle() const
{
	return m_sharedNativeHandle.relaxedRead();
}

std::size_t DetailedTexture::getWidthPx() const
{
	return m_resource ? m_resource->getWidthPx() : 0;
}

std::size_t DetailedTexture::getHeightPx() const
{
	return m_resource ? m_resource->getHeightPx() : 0;
}

std::size_t DetailedTexture::numLayers() const
{
	return m_resource ? m_resource->numLayers() : 0;
}

GHITextureHandle DetailedTexture::getGHITextureHandle() const
{
	return m_resource ? m_resource->getGHITextureHandle() : GHITextureHandle{};
}

}// end namespace ph::editor::render
