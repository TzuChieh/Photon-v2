#include "RenderCore/GHIThread.h"
#include "RenderCore/GraphicsContext.h"
#include "RenderCore/Null/NullContext.h"

#include <Common/logging.h>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(GHIThread, RenderCore);

GHIThread::GHIThread()
	: Base()
	, m_ctx(nullptr)
	, m_nullCtx(std::make_unique<NullContext>())
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
	addWork(
		[this](GraphicsContext& /* ctx */)
		{
			m_frameTimer.start();
		});
}

void GHIThread::onEndFrame()
{
	// Swap buffer at the end of end frame
	addWork(
		[](GraphicsContext& ctx)
		{
			ctx.getGHI().swapBuffers();
		});

	addWork(
		[this](GraphicsContext& /* ctx */)
		{
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
