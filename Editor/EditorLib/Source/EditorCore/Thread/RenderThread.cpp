#include "EditorCore/Thread/RenderThread.h"
#include "EditorCore/Thread/GHIThread.h"

#include <Common/logging.h>
#include <Common/assertion.h>

#include <utility>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(RenderThread, EditorCore);

RenderThread::RenderThread()
	: Base()
	, m_scene()
	, m_ghiThread(nullptr)
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
	// TODO
}

void RenderThread::onAsyncWorkerStart()
{
	// TODO
}

void RenderThread::onAsyncWorkerStop()
{
	// TODO
}

void RenderThread::onBeginFrame(const std::size_t frameNumber, const std::size_t frameCycleIndex)
{
	// TODO
}

void RenderThread::onEndFrame()
{
	// TODO
}

void RenderThread::addGHIUpdateWork(GHI* updatedGHI)
{
	// TODO
	/*addWork(
		[this, updatedGHI = std::move(updatedGHI)](RTRScene& scene)
		{
			m_updatedGHI = std::move(updatedGHI);
		});*/
}

}// end namespace ph::editor
