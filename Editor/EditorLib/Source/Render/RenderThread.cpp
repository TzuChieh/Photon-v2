#include "Render/RenderThread.h"
#include "Render/UpdateContext.h"
#include "EditorCore/Thread/Threads.h"
#include "Render/System.h"
#include "RenderCore/GHIThreadCaller.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <utility>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(RenderThread, Render);

RenderThread::RenderThread()

	: Base()

	, m_system(nullptr)
	, m_ghiThread()
	, m_newGraphicsCtx(nullptr)
	, m_frameTimer()
	, m_frameTimeMs(0)
{}

RenderThread::~RenderThread()
{}

void RenderThread::onAsyncProcessWork(const Work& work)
{
	PH_ASSERT(m_system);

	work(*m_system);
}

void RenderThread::onAsyncWorkerStart()
{
	PH_LOG(RenderThread, "thread started");

	m_system = std::make_unique<render::System>();

	// Must start here--render thread should be the parent thread of GHI thread
	m_ghiThread.startWorker();
}

void RenderThread::onAsyncWorkerStop()
{
	PH_ASSERT(m_system);

	// Waits for all previous GHI works to finish
	m_ghiThread.beginFrame();

	// Destroy resources just like how we did in `onEndFrame()`
	for(auto& scene : m_system->scenes)
	{
		scene->destroyPendingResources();
	}

	m_ghiThread.requestWorkerStop();
	m_ghiThread.endFrame();

	m_ghiThread.waitForWorkerToStop();

	m_system = nullptr;

	PH_LOG(RenderThread, "thread stopped");
}

void RenderThread::onBeginFrame()
{
	addWork(
		[this](render::System& /* sys */)
		{
			m_frameTimer.start();
		});

	// Update context need to be updated first for render thread

	const auto frameInfo = getFrameInfo();

	render::UpdateContext updateCtx;
	updateCtx.frameNumber = frameInfo.frameNumber;
	updateCtx.frameCycleIndex = frameInfo.frameCycleIndex;

	addWork(
		[updateCtx](render::System& sys)
		{
			sys.updateCtx = updateCtx;
		});

	addWork(
		[this](render::System& /* sys */)
		{
			beforeFirstRenderWorkSubmission();
		});
}

void RenderThread::onEndFrame()
{
	addWork(
		[this](render::System& /* sys */)
		{
			afterLastRenderWorkSubmission();
		});

	addWork(
		[](render::System& sys)
		{
			for(auto& scene : sys.scenes)
			{
				scene->updateCustomRenderContents(sys.updateCtx);
			}
		});

	// GHI work submission
	addWork(
		[this](render::System& sys)
		{
			// Placement of GHI begin frame is important--it waits for all previous GHI works to finish
			m_ghiThread.beginFrame();

			// Destory resources once we are sure the GHI thread is done accessing them
			// (with memory effects on GHI thread made visible)
			for(auto& scene : sys.scenes)
			{
				scene->destroyPendingResources();
			}

			// If non-null, a graphics context switch is pending
			if(m_newGraphicsCtx)
			{
				m_ghiThread.addContextSwitchWork(m_newGraphicsCtx);
				sys.setGraphicsContext(m_newGraphicsCtx);

				m_newGraphicsCtx = nullptr;
			}

			GHIThreadCaller caller(m_ghiThread);

			for(auto& scene : sys.scenes)
			{
				scene->setupGHIForPendingResources(caller);
				scene->createGHICommandsForCustomRenderContents(caller);
				scene->cleanupGHIForPendingResources(caller);
			}

			m_ghiThread.endFrame();
		});

	addWork(
		[this](render::System& /* sys */)
		{
			m_frameTimer.stop();
			m_frameTimeMs.store(m_frameTimer.getDeltaMs<float32>(), std::memory_order_relaxed);
		});
}

void RenderThread::addGraphicsContextSwitchWork(GraphicsContext* const newCtx)
{
	addWork(
		[this, newCtx](render::System& /* sys */)
		{
			m_newGraphicsCtx = newCtx;
		});
}

void RenderThread::beforeFirstRenderWorkSubmission()
{
	PH_ASSERT(Threads::isOnRenderThread());

	
	// TODO
}

void RenderThread::afterLastRenderWorkSubmission()
{
	PH_ASSERT(Threads::isOnRenderThread());

	m_system->waitAllFileReadingWorks();
}

}// end namespace ph::editor
