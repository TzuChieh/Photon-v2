#include "EditorCore/Thread/GHIThread.h"
#include "RenderCore/NullGHI.h"

#include <Common/logging.h>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(GHIThread, EditorCore);

GHIThread::GHIThread()
	: Base()
	, m_ghi(nullptr)
	, m_defaultGHI(std::make_unique<NullGHI>())
{
	PH_LOG(GHIThread, "thread created");
}

GHIThread::~GHIThread()
{
	PH_LOG(GHIThread, "thread destroyed");
}

void GHIThread::onAsyncWorkerStart()
{
	// TODO: load ghi
}

void GHIThread::onAsyncWorkerStop()
{
	// TODO: unload ghi
}

void GHIThread::onAsyncProcessWork(const Work& work)
{
	PH_ASSERT(m_ghi);

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

void GHIThread::addSetGHIWork(GHI* ghi)
{
	// TODO: setGHI
}

void GHIThread::setGHI(GHI* ghi)
{
	// TODO: unload previous, load new
}

}// end namespace ph::editor
