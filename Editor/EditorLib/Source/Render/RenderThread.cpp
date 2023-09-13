#include "Render/RenderThread.h"
#include "Render/UpdateContext.h"
#include "EditorCore/Thread/Threads.h"
#include "Render/System.h"
#include "RenderCore/GHIThreadCaller.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Utility/exception.h>

#include <utility>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(RenderThread, Render);

RenderThread::RenderThread()

	: Base()

	, m_system(nullptr)
	, m_ghiThread()
	, m_graphicsCtx(nullptr)
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

	if(!m_graphicsCtx)
	{
		throw IllegalOperationException(
			"Render thread starting without valid graphics context.");
	}

	// Must start here--render thread should be the parent thread of GHI thread
	m_ghiThread.startWorker();

	m_ghiThread.beginFrame();

	// Switch to our target graphics context. Render thread does not allow changing graphics context
	// during frame updates later, so all graphics initialization work can be done here.
	PH_ASSERT(m_graphicsCtx);
	m_ghiThread.addContextSwitchWork(m_graphicsCtx);

	// Make sure graphics initialization works are done
	m_ghiThread.waitAllWorks();

	m_ghiThread.endFrame();

	// Initialize the rendering system after we got a functional graphics context
	m_system = std::make_unique<render::System>(*m_graphicsCtx);
}

void RenderThread::onAsyncWorkerStop()
{
	PH_ASSERT(m_system);

	// Remove all scenes (note that we are iterating while removing)
	while(!m_system->getScenes().empty())
	{
		m_system->removeScene(m_system->getScenes().back());
	}

	// Wait for all previous GHI works to finish
	m_ghiThread.beginFrame();

	// Cleanup removed scenes just like how we did in `onEndFrame()`
	{
		for(render::Scene* scene : m_system->getRemovedScenes())
		{
			GHIThreadCaller caller(m_ghiThread);
			scene->cleanupGHIForPendingResources(caller);
			scene->destroyPendingResources();
		}

		render::SystemController(*m_system).clearRemovedScenes();
	}

	// TODO: cleanup scene

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
			beforeFirstRenderWorkInFrame();
		});
}

void RenderThread::onEndFrame()
{
	addWork(
		[this](render::System& /* sys */)
		{
			afterLastRenderWorkInFrame();
		});

	addWork(
		[](render::System& sys)
		{
			for(render::Scene* scene : sys.getScenes())
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
			{
				for(render::Scene* scene : sys.getScenes())
				{
					scene->destroyPendingResources();
				}
			}

			// Cleanup removed scenes
			{
				for(render::Scene* scene : sys.getRemovedScenes())
				{
					GHIThreadCaller caller(m_ghiThread);
					scene->cleanupGHIForPendingResources(caller);
					scene->destroyPendingResources();
				}

				render::SystemController(sys).clearRemovedScenes();
			}

			for(render::Scene* scene : sys.getScenes())
			{
				GHIThreadCaller caller(m_ghiThread);
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

void RenderThread::beforeFirstRenderWorkInFrame()
{
	PH_ASSERT(Threads::isOnRenderThread());

	// TODO
}

void RenderThread::afterLastRenderWorkInFrame()
{
	PH_ASSERT(Threads::isOnRenderThread());

	render::SystemController sys(*m_system);

	sys.waitAllFileReadingWorks();
	sys.processQueries();
}

void RenderThread::setGraphicsContext(GraphicsContext* graphicsCtx)
{
	if(!graphicsCtx || m_graphicsCtx)
	{
		throw IllegalOperationException(
			"Attempting to reset graphics context. Context can only be set once.");
	}

	m_graphicsCtx = graphicsCtx;
}

}// end namespace ph::editor
