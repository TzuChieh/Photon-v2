#include "EditorCore/Thread/RenderThread.h"
#include "RenderCore/RenderThreadUpdateContext.h"
#include "EditorCore/Thread/GHIThreadCaller.h"

#include <Common/logging.h>
#include <Common/assertion.h>

#include <utility>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(RenderThread, EditorCore);

RenderThread::RenderThread()
	: Base()
	, m_renderData()
	, m_ghiThread()
	, m_updatedGHI(nullptr)
{
	PH_LOG(RenderThread, "thread created");
}

RenderThread::~RenderThread()
{
	PH_LOG(RenderThread, "thread destroyed");
}

void RenderThread::onAsyncProcessWork(const Work& work)
{
	work(m_renderData);
}

void RenderThread::onAsyncWorkerStart()
{
	// Must start here--render thread should be the parent thread of GHI thread
	m_ghiThread.startWorker();
}

void RenderThread::onAsyncWorkerStop()
{
	m_ghiThread.beginFrame();
	m_ghiThread.requestWorkerStop();
	m_ghiThread.endFrame();

	m_ghiThread.waitForWorkerToStop();
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
			renderData.scene.update(renderData.updateCtx);
		});

	// GHI work submission
	addWork(
		[this](RenderData& renderData)
		{
			m_ghiThread.beginFrame();

			// If it is non-null, a GHI update is pending
			if(m_updatedGHI)
			{
				m_ghiThread.addSetGHIWork(m_updatedGHI);
				m_updatedGHI = nullptr;
			}

			GHIThreadCaller caller(m_ghiThread);
			renderData.scene.createGHICommands(caller);

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
