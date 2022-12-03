#include "Render/RenderThread.h"
#include "Render/RenderThreadUpdateContext.h"
#include "RenderCore/GHIThreadCaller.h"

#include <Common/logging.h>
#include <Common/assertion.h>

#include <utility>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(RenderThread, Render);

RenderThread::RenderThread()
	: Base()
	, m_renderData(std::nullopt)
	, m_ghiThread()
	, m_updatedGHI(nullptr)
{}

RenderThread::~RenderThread()
{}

void RenderThread::onAsyncProcessWork(const Work& work)
{
	PH_ASSERT(m_renderData.has_value());

	work(*m_renderData);
}

void RenderThread::onAsyncWorkerStart()
{
	PH_LOG(RenderThread, "thread started");

	m_renderData = RenderData();

	// Must start here--render thread should be the parent thread of GHI thread
	m_ghiThread.startWorker();
}

void RenderThread::onAsyncWorkerStop()
{
	PH_ASSERT(m_renderData.has_value());

	m_ghiThread.beginFrame();

	// Destroy resources just like how we did in `onEndFrame()`
	m_renderData->scene.destroyPendingResources();

	m_ghiThread.requestWorkerStop();
	m_ghiThread.endFrame();

	m_ghiThread.waitForWorkerToStop();

	m_renderData = std::nullopt;

	PH_LOG(RenderThread, "thread stopped");
}

void RenderThread::onBeginFrame()
{
	// Update context need to be updated first for render thread

	const auto frameInfo = getFrameInfo();

	RenderThreadUpdateContext updateCtx;
	updateCtx.frameNumber = frameInfo.frameNumber;
	updateCtx.frameCycleIndex = frameInfo.frameCycleIndex;

	addWork(
		[updateCtx](RenderData& renderData)
		{
			renderData.updateCtx = updateCtx;
		});

	addWork(
		[this](RenderData& /* renderData */)
		{
			beginProcessFrame();
		});

	// TODO
}

void RenderThread::onEndFrame()
{
	addWork(
		[](RenderData& renderData)
		{
			renderData.scene.updateCustomRenderContents(renderData.updateCtx);
		});

	// GHI work submission
	addWork(
		[this](RenderData& renderData)
		{
			// Placement of GHI begin frame is important--it waits for all previous GHI works to finish
			m_ghiThread.beginFrame();

			// Destory resources once we are sure the GHI thread is done accessing them
			// (and memory effects on GHI thread are made visible)
			renderData.scene.destroyPendingResources();

			// If it is non-null, a GHI update is pending
			if(m_updatedGHI)
			{
				m_ghiThread.addSetGHIWork(m_updatedGHI);
				m_updatedGHI = nullptr;
			}

			GHIThreadCaller caller(m_ghiThread);

			renderData.scene.setupGHIForPendingResources(caller);
			renderData.scene.createGHICommandsForCustomRenderContents(caller);
			renderData.scene.cleanupGHIForPendingResources(caller);

			m_ghiThread.endFrame();
		});

	addWork(
		[this](RenderData& /* renderData */)
		{
			endProcessFrame();
		});
}

void RenderThread::addGHIUpdateWork(GHI* const updatedGHI)
{
	addWork(
		[this, updatedGHI](RenderData& /* renderData */)
		{
			m_updatedGHI = updatedGHI;
		});
}

void RenderThread::beginProcessFrame()
{
	// TODO
}

void RenderThread::endProcessFrame()
{
	// TODO
}

}// end namespace ph::editor
