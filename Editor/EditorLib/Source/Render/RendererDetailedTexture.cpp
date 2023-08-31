#include "Render/RendererDetailedTexture.h"
#include "RenderCore/ghi_infos.h"
#include "RenderCore/GHIThreadCaller.h"
#include "RenderCore/GraphicsContext.h"
#include "RenderCore/GHI.h"

#include <Frame/PictureData.h>

#include <utility>

namespace ph::editor
{

RendererDetailedTexture::RendererDetailedTexture(
	std::unique_ptr<RendererTexture> resource)

	: RendererTexture()

	, m_resource(std::move(resource))
	, m_sharedNativeHandle()
{}

void RendererDetailedTexture::setupGHI(GHIThreadCaller& caller)
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
				GHITextureNativeHandle nativeHandle = ctx.getGHI().getTextureNativeHandle(handle);
				m_sharedNativeHandle.relaxedWrite(nativeHandle);
			}
		});
}

void RendererDetailedTexture::cleanupGHI(GHIThreadCaller& caller)
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

std::optional<GHITextureNativeHandle> RendererDetailedTexture::tryGetNativeHandle() const
{
	return m_sharedNativeHandle.relaxedRead();
}

std::size_t RendererDetailedTexture::getWidthPx() const
{
	return m_resource ? m_resource->getWidthPx() : 0;
}

std::size_t RendererDetailedTexture::getHeightPx() const
{
	return m_resource ? m_resource->getHeightPx() : 0;
}

std::size_t RendererDetailedTexture::numLayers() const
{
	return m_resource ? m_resource->numLayers() : 0;
}

GHITextureHandle RendererDetailedTexture::getGHITextureHandle() const
{
	return m_resource ? m_resource->getGHITextureHandle() : GHITextureHandle{};
}

}// end namespace ph::editor
