#include "Render/Texture2D.h"
#include "RenderCore/GHIThreadCaller.h"
#include "RenderCore/GraphicsContext.h"
#include "RenderCore/GraphicsObjectManager.h"
#include "RenderCore/GHI.h"

#include <Frame/PictureData.h>

#include <utility>

namespace ph::editor::render
{

Texture2D::Texture2D(
	const ghi::TextureFormatInfo& format,
	std::unique_ptr<PictureData> textureData)

	: TextureResource()

	, m_sizePx(0)
	, m_format(format)
	, m_textureHandle()
	, m_textureData(std::move(textureData))
{
	if(m_textureData)
	{
		m_sizePx = m_textureData->getSizePx().losslessCast<uint32>();
	}
}

Texture2D::~Texture2D()
{}

void Texture2D::setupGHI(GHIThreadCaller& caller)
{
	if(!m_textureData)
	{
		return;
	}

	/*caller.add(
		[this](GraphicsContext& ctx)
		{
			PH_ASSERT(!m_textureHandle);

			GHIInfoTextureDesc desc;
			desc.format = m_format;
			desc.setSize2D(m_sizePx);
			m_textureHandle = ctx.getObjectManager().createTexture(desc);

			ctx.getObjectManager().uploadPixelData(
				m_textureHandle, 
				m_textureData->getBytes(),
				translate_to<EGHIPixelComponent>(m_textureData->getComponentType()));
			m_textureData = nullptr;
		});*/
}

void Texture2D::cleanupGHI(GHIThreadCaller& caller)
{
	caller.add(
		[this](ghi::GraphicsContext& ctx)
		{
			// Note: Always decrement reference count on GHI thread--one of this call will free 
			// the GHI resource, and it must be done on GHI thread
			ctx.getObjectManager().removeTexture(m_textureHandle);
			m_textureHandle = ghi::TextureHandle{};
		});
}

}// end namespace ph::editor::render
