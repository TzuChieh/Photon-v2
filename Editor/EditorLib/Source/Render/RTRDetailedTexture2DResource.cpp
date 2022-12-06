#include "Render/RTRDetailedTexture2DResource.h"
#include "RenderCore/GHIThreadCaller.h"

#include <Frame/PictureData.h>

#include <utility>

namespace ph::editor
{

RTRDetailedTexture2DResource::RTRDetailedTexture2DResource(
	const GHIInfoTextureFormat& format,
	std::unique_ptr<PictureData> textureData)

	: RTRTexture2DResource(format, std::move(textureData))

	, m_sharedNativeHandle(std::nullopt)
{}

void RTRDetailedTexture2DResource::setupGHI(GHIThreadCaller& caller)
{
	Base::setupGHI(caller);

	caller.add(
		[this](GHI& /* ghi */)
		{
			const auto& ghiTexture = getGHITexture();

			// Generally this should be true since the call is issued after base GHI creation
			if(ghiTexture)
			{
				m_sharedNativeHandle.relaxedWrite(ghiTexture->getNativeHandle());
			}
		});
}

void RTRDetailedTexture2DResource::cleanupGHI(GHIThreadCaller& caller)
{
	Base::cleanupGHI(caller);
}

std::optional<GHITexture2D::NativeHandle> RTRDetailedTexture2DResource::tryGetNativeHandle() const
{
	return m_sharedNativeHandle.relaxedRead();
}

}// end namespace ph::editor
