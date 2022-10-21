#include "EditorCore/Thread/GHIThread.h"

#include <Common/logging.h>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(GHIThread, EditorCore);

GHIThread::GHIThread()
	: Base()
{
	PH_LOG(GHIThread, "thread started");
}

GHIThread::~GHIThread()
{
	PH_LOG(GHIThread, "thread ended");
}

void GHIThread::onAsyncProcessWork(const Work& work)
{
	//PH_ASSERT(Threads::isOnRenderThread());

	// TODO
}

void GHIThread::onBeginFrame(const std::size_t frameNumber, const std::size_t frameCycleIndex)
{
	// TODO
}

void GHIThread::onEndFrame()
{
	// TODO
}

}// end namespace ph::editor
