#include "Render/RTRTexture2DResource.h"
#include "RenderCore/GHIThreadCaller.h"
#include "RenderCore/GHI.h"

#include <Frame/PictureData.h>

#include <utility>

namespace ph::editor
{

RTRTexture2DResource::RTRTexture2DResource(
	const GHIInfoTextureFormat& format,
	std::unique_ptr<PictureData> textureData)

	: RTRResource()

	, m_sizePx(0)
	, m_format(format)
	, m_ghiTexture(nullptr)
	, m_textureData(std::move(textureData))
{
	if(m_textureData)
	{
		m_sizePx = m_textureData->getSizePx().safeCast<uint32>();
	}
}

RTRTexture2DResource::~RTRTexture2DResource() = default;

void RTRTexture2DResource::setupGHI(GHIThreadCaller& caller)
{
	PH_ASSERT(!m_ghiTexture);

	if(!m_textureData)
	{
		return;
	}

	caller.add(
		[this](GHI& ghi)
		{
			m_ghiTexture = ghi.createTexture2D(m_format, m_sizePx);

			m_ghiTexture->upload(
				m_textureData->getData(),
				m_textureData->numBytesInData(),
				from_picture_pixel_component(m_textureData->getComponentType()));

			m_textureData = nullptr;
		});
}

void RTRTexture2DResource::cleanupGHI(GHIThreadCaller& caller)
{
	PH_ASSERT(m_ghiTexture);

	caller.add(
		[this](GHI& ghi)
		{
			// Note: Always decrement reference count on GHI thread--one of this call will free the GHI
			// resource, and it must be done on GHI thread
			m_ghiTexture = nullptr;
		});
}

}// end namespace ph::editor
