#include "Render/RTRDetailedTexture2DResource.h"
#include "RenderCore/GHIThreadCaller.h"

namespace ph::editor
{

RTRDetailedTexture2DResource::RTRDetailedTexture2DResource(
	const math::Vector2UI& sizePx, 
	const GHIInfoTextureFormat& format)

	: RTRTexture2DResource(sizePx, format)
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
