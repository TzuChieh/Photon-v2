#include "RenderCore/GHIThread.h"
#include "RenderCore/GraphicsContext.h"
#include "RenderCore/Null/NullContext.h"
#include "RenderCore/GraphicsObjectManager.h"
#include "RenderCore/GraphicsMemoryManager.h"

#include <Common/logging.h>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(GHIThread, RenderCore);

GHIThread::GHIThread()
	: Base()
	, m_ctx(nullptr)
	, m_nullCtx(std::make_unique<NullContext>())
	, m_updateCtx()
	, m_frameTimer()
	, m_frameTimeMs(0)
{}

GHIThread::~GHIThread()
{
	// Should already been unset
	PH_ASSERT(!m_ctx);
}

void GHIThread::onAsyncWorkerStart()
{
	PH_LOG(GHIThread, "thread started");

	setContext(m_nullCtx.get());
}

void GHIThread::onAsyncWorkerStop()
{
	setContext(nullptr);

	PH_LOG(GHIThread, "thread stopped");
}

void GHIThread::onAsyncProcessWork(const Work& work)
{
	PH_ASSERT(m_ctx);

	work(*m_ctx);
}

void GHIThread::onBeginFrame()
{
	const auto frameInfo = getFrameInfo();
	m_updateCtx.frameNumber = frameInfo.frameNumber;

	addWork(
		[this](GraphicsContext& ctx)
		{
			m_frameTimer.start();

			// Begin update before object manager
			ctx.getMemoryManager().beginFrameUpdate(m_updateCtx);

			ctx.getObjectManager().beginFrameUpdate(m_updateCtx);
		});
}

void GHIThread::onEndFrame()
{
	addWork(
		[this](GraphicsContext& ctx)
		{
			ctx.getObjectManager().endFrameUpdate(m_updateCtx);

			// End update after object manager
			ctx.getMemoryManager().endFrameUpdate(m_updateCtx);

			// Swap buffer at the end of end frame
			ctx.getGHI().swapBuffers();

			m_frameTimer.stop();
			m_frameTimeMs.store(m_frameTimer.getDeltaMs<float32>(), std::memory_order_relaxed);
		});
}

void GHIThread::addSetContextWork(GraphicsContext* const inCtx)
{
	addWork(
		[this, inCtx](GraphicsContext& /* ctx */)
		{
			setContext(inCtx);
		});
}

void GHIThread::setContext(GraphicsContext* const inCtx)
{
	// Nothing to do if the contexts are the same
	if(m_ctx == inCtx)
	{
		return;
	}

	if(m_ctx)
	{
		m_ctx->getGHI().unload();
	}
	
	m_ctx = inCtx;

	if(m_ctx)
	{
		m_ctx->getGHI().load();
	}
}

}// end namespace ph::editor
