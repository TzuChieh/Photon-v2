#include "EditorCore/Thread/RenderThread.h"
#include "EditorCore/Thread/GHIThread.h"
#include "RenderCore/RenderThreadUpdateContext.h"

#include <Common/logging.h>
#include <Common/assertion.h>

#include <utility>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(RenderThread, EditorCore);

RenderThread::RenderThread()
	: Base()
	, m_scene()
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
	work(m_scene);
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
	addWork(
		[this](RTRScene& /* scene */)
		{
			beginProcessFrame();
		});

	// TODO
}

void RenderThread::onEndFrame()
{
	const auto frameInfo = getFrameInfo();

	RenderThreadUpdateContext updateCtx;
	updateCtx.frameNumber = frameInfo.frameNumber;
	updateCtx.frameCycleIndex = frameInfo.frameCycleIndex;

	addWork(
		[updateCtx](RTRScene& scene)
		{
			scene.update(updateCtx);
		});

	// GHI work submission
	addWork(
		[this](RTRScene& scene)
		{
			m_ghiThread.beginFrame();
			scene.createGHICommands();
			m_ghiThread.endFrame();
		});

	addWork(
		[this](RTRScene& /* scene */)
		{
			endProcessFrame();
		});
}

void RenderThread::addGHIUpdateWork(GHI* const updatedGHI)
{
	addWork(
		[this, updatedGHI](RTRScene& /* scene */)
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
