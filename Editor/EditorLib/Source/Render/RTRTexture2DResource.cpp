#include "Render/RTRTexture2DResource.h"
#include "RenderCore/GHIThreadCaller.h"
#include "RenderCore/GHI.h"

namespace ph::editor
{

RTRTexture2DResource::RTRTexture2DResource(const math::Vector2UI& sizePx, const GHIInfoTextureFormat& format)
	: RTRResource()
	, m_sizePx(sizePx)
	, m_format(format)
	, m_ghiTexture(nullptr)
{}

void RTRTexture2DResource::setupGHI(GHIThreadCaller& caller)
{
	PH_ASSERT(!m_ghiTexture);

	caller.add(
		[this](GHI& ghi)
		{
			m_ghiTexture = ghi.createTexture2D(m_sizePx, m_format);
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
