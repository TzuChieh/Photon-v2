#include "Render/RendererDetailedTexture.h"
#include "RenderCore/GHIThreadCaller.h"

#include <Frame/PictureData.h>

#include <utility>

namespace ph::editor
{

RendererDetailedTexture::RendererDetailedTexture(
	std::unique_ptr<RendererTexture> resource)

	: RendererTexture()

	, m_resource(std::move(resource))
	, m_sharedNativeHandle(std::nullopt)
{}

void RendererDetailedTexture::setupGHI(GHIThreadCaller& caller)
{
	if(m_resource)
	{
		m_resource->setupGHI(caller);
	}

	caller.add(
		[this](GraphicsContext& /* ctx */)
		{
			GHITexture* const ghiTexture = getGHITexture();

			// Generally this should be true since the call is issued after base resource GHI 
			// creation (unless base resource is empty)
			if(ghiTexture)
			{
				m_sharedNativeHandle.relaxedWrite(ghiTexture->getNativeHandle());
			}
		});
}

void RendererDetailedTexture::cleanupGHI(GHIThreadCaller& caller)
{
	caller.add(
		[this](GraphicsContext& /* ctx */)
		{
			m_sharedNativeHandle.relaxedWrite(std::nullopt);
		});

	if(m_resource)
	{
		m_resource->cleanupGHI(caller);
	}
}

std::optional<GHITexture::NativeHandle> RendererDetailedTexture::tryGetNativeHandle() const
{
	return m_sharedNativeHandle.relaxedRead();
}

GHITexture* RendererDetailedTexture::getGHITexture() const
{
	return m_resource ? m_resource->getGHITexture() : nullptr;
}

std::shared_ptr<GHITexture> RendererDetailedTexture::getGHITextureResource() const
{
	return m_resource ? m_resource->getGHITextureResource() : nullptr;
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

}// end namespace ph::editor
