#include "Render/RendererFramebuffer.h"
#include "RenderCore/GHIThreadCaller.h"
#include "RenderCore/GHI.h"

#include <utility>

namespace ph::editor
{

RendererFramebuffer::RendererFramebuffer(
	const GHIInfoFramebufferAttachment& attachments)

	: RendererResource()

	, m_attachments(attachments)
	, m_ghiFramebuffer(nullptr)
{}

RendererFramebuffer::~RendererFramebuffer()
{
	// Must have been released by GHI thread
	PH_ASSERT(!m_ghiFramebuffer);
}

void RendererFramebuffer::setupGHI(GHIThreadCaller& caller)
{
	caller.add(
		[this](GHI& ghi)
		{
			PH_ASSERT(!m_ghiFramebuffer);

			m_ghiFramebuffer = ghi.createFramebuffer(m_attachments);

			// TODO: pre clear?
		});
}

void RendererFramebuffer::cleanupGHI(GHIThreadCaller& caller)
{
	caller.add(
		[this](GHI& ghi)
		{
			if(m_ghiFramebuffer)
			{
				// Note: Always decrement reference count on GHI thread--one of this call will free 
				// the GHI resource, and it must be done on GHI thread
				m_ghiFramebuffer = nullptr;
			}
		});
}

}// end namespace ph::editor
