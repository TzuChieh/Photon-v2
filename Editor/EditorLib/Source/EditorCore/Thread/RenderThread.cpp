#include "EditorCore/Thread/RenderThread.h"
#include "EditorCore/RenderThreadResource.h"

#include <Common/logging.h>
#include <Common/assertion.h>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(RenderThread, EditorCore);

RenderThread::RenderThread()
	: Base()
	, m_scene()
	, m_ghiThread(nullptr)
{
	PH_LOG(RenderThread, "thread started");
}

RenderThread::~RenderThread()
{
	PH_LOG(RenderThread, "thread ended");
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

}// end namespace ph::editor
