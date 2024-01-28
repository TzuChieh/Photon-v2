#include "RenderCore/GHIThread.h"
#include "RenderCore/GraphicsContext.h"
#include "RenderCore/Null/NullContext.h"
#include "RenderCore/GraphicsObjectManager.h"
#include "RenderCore/GraphicsMemoryManager.h"
#include "EditorCore/Thread/Threads.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Common/profiling.h>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(GHIThread, RenderCore);

GHIThread::GHIThread()
	: Base()
	, m_ctx(nullptr)
	, m_nullCtx(std::make_unique<ghi::NullContext>())
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
	PH_PROFILE_NAME_THIS_THREAD("GHI thread");
	PH_LOG(GHIThread, Note, "thread started");

	switchContext(m_nullCtx.get());
}

void GHIThread::onAsyncWorkerStop()
{
	switchContext(nullptr);

	PH_LOG(GHIThread, Note, "thread stopped");
}

void GHIThread::onAsyncProcessWork(const Work& work)
{
	PH_ASSERT(m_ctx);

	work(*m_ctx);
}

void GHIThread::onBeginFrame()
{
	PH_PROFILE_SCOPE();

	const auto frameInfo = getFrameInfo();
	m_updateCtx.frameNumber = frameInfo.frameNumber;

	addWork(
		[this](ghi::GraphicsContext& ctx)
		{
			m_frameTimer.start();

			ctx.beginFrameUpdate(m_updateCtx);
		});
}

void GHIThread::onEndFrame()
{
	PH_PROFILE_SCOPE();

	addWork(
		[this](ghi::GraphicsContext& ctx)
		{
			ctx.endFrameUpdate(m_updateCtx);

			// Swap buffer at the end of end frame
			ctx.getGHI().swapBuffers();

			m_frameTimer.stop();
			m_frameTimeMs.store(m_frameTimer.getDeltaMs<float32>(), std::memory_order_relaxed);
		});
}

void GHIThread::addContextSwitchWork(ghi::GraphicsContext* const newCtx)
{
	addWork(
		[this, newCtx](ghi::GraphicsContext& /* ctx */)
		{
			switchContext(newCtx);
		});
}

void GHIThread::switchContext(ghi::GraphicsContext* const newCtx)
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
