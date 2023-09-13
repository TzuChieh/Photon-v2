#include "RenderCore/GHIThread.h"
#include "RenderCore/GraphicsContext.h"
#include "RenderCore/Null/NullContext.h"
#include "RenderCore/GraphicsObjectManager.h"
#include "RenderCore/GraphicsMemoryManager.h"
#include "EditorCore/Thread/Threads.h"

#include <Common/assertion.h>
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

	switchContext(m_nullCtx.get());
}

void GHIThread::onAsyncWorkerStop()
{
	switchContext(nullptr);

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

			ctx.beginFrameUpdate(m_updateCtx);
		});
}

void GHIThread::onEndFrame()
{
	addWork(
		[this](GraphicsContext& ctx)
		{
			ctx.endFrameUpdate(m_updateCtx);

			// Swap buffer at the end of end frame
			ctx.getGHI().swapBuffers();

			m_frameTimer.stop();
			m_frameTimeMs.store(m_frameTimer.getDeltaMs<float32>(), std::memory_order_relaxed);
		});
}

void GHIThread::addContextSwitchWork(GraphicsContext* const newCtx)
{
	addWork(
		[this, newCtx](GraphicsContext& /* ctx */)
		{
			switchContext(newCtx);
		});
}

void GHIThread::switchContext(GraphicsContext* const newCtx)
{
	PH_ASSERT(isOnGHIThread());

	// Nothing to do if the contexts are the same
	if(m_ctx == newCtx)
	{
		return;
	}

	if(m_ctx)
	{
		// Keep begin/end updates in pair for the old context
		m_ctx->endFrameUpdate(m_updateCtx);

		// Swap buffers the last time before GHI unload
		m_ctx->getGHI().swapBuffers();

		m_ctx->unload();
	}
	
	m_ctx = newCtx;

	if(newCtx)
	{
		newCtx->load();

		// Keep begin/end updates in pair for the new context
		newCtx->beginFrameUpdate(m_updateCtx);
	}
}

}// end namespace ph::editor
