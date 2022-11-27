#include "RenderCore/RTRTexture2D.h"
#include "EditorCore/Thread/GHIThreadCaller.h"
#include "RenderCore/GHI.h"

namespace ph::editor
{

RTRTexture2D::RTRTexture2D(const math::Vector2UI& sizePx, const GHIInfoTextureFormat& format)
	: RTRResource()
	, m_sizePx(sizePx)
	, m_format(format)
	, m_ghiTexture(nullptr)
{}

void RTRTexture2D::createGHIResource(GHIThreadCaller& caller)
{
	PH_ASSERT(!m_ghiTexture);

	caller.add(
		[this](GHI& ghi)
		{
			m_ghiTexture = ghi.createTexture2D(m_sizePx, m_format);
		});
}

void RTRTexture2D::destroyGHIResource(GHIThreadCaller& caller)
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
