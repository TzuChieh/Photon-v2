#include "Render/RTRDetailedTextureResource.h"
#include "RenderCore/GHIThreadCaller.h"

#include <Frame/PictureData.h>

#include <utility>

namespace ph::editor
{

RTRDetailedTextureResource::RTRDetailedTextureResource(
	std::unique_ptr<RTRTextureResource> resource)

	: RTRTextureResource()

	, m_resource(std::move(resource))
	, m_sharedNativeHandle(std::nullopt)
{}

void RTRDetailedTextureResource::setupGHI(GHIThreadCaller& caller)
{
	if(m_resource)
	{
		m_resource->setupGHI(caller);
	}

	caller.add(
		[this](GHI& /* ghi */)
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

void RTRDetailedTextureResource::cleanupGHI(GHIThreadCaller& caller)
{
	caller.add(
		[this](GHI& /* ghi */)
		{
			m_sharedNativeHandle.relaxedWrite(std::nullopt);
		});

	if(m_resource)
	{
		m_resource->cleanupGHI(caller);
	}
}

std::optional<GHITexture::NativeHandle> RTRDetailedTextureResource::tryGetNativeHandle() const
{
	return m_sharedNativeHandle.relaxedRead();
}

GHITexture* RTRDetailedTextureResource::getGHITexture() const
{
	return m_resource ? m_resource->getGHITexture() : nullptr;
}

std::shared_ptr<GHITexture> RTRDetailedTextureResource::getGHITextureResource() const
{
	return m_resource ? m_resource->getGHITextureResource() : nullptr;
}

std::size_t RTRDetailedTextureResource::getWidthPx() const
{
	return m_resource ? m_resource->getWidthPx() : 0;
}

std::size_t RTRDetailedTextureResource::getHeightPx() const
{
	return m_resource ? m_resource->getHeightPx() : 0;
}

std::size_t RTRDetailedTextureResource::numLayers() const
{
	return m_resource ? m_resource->numLayers() : 0;
}

}// end namespace ph::editor
