#include "Render/RendererTexture2D.h"
#include "RenderCore/GHIThreadCaller.h"
#include "RenderCore/GraphicsContext.h"
#include "RenderCore/GraphicsObjectManager.h"
#include "RenderCore/GHI.h"

#include <Frame/PictureData.h>

#include <utility>

namespace ph::editor
{

RendererTexture2D::RendererTexture2D(
	const GHIInfoTextureFormat& format,
	std::unique_ptr<PictureData> textureData)

	: RendererTexture()

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

RendererTexture2D::~RendererTexture2D()
{}

void RendererTexture2D::setupGHI(GHIThreadCaller& caller)
{
	if(!m_textureData)
	{
		return;
	}

	caller.add(
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
		});
}

void RendererTexture2D::cleanupGHI(GHIThreadCaller& caller)
{
	caller.add(
		[this](GraphicsContext& ctx)
		{
			// Note: Always decrement reference count on GHI thread--one of this call will free 
			// the GHI resource, and it must be done on GHI thread
			ctx.getObjectManager().removeTexture(m_textureHandle);
			m_textureHandle = GHITextureHandle{};
		});
}

}// end namespace ph::editor
