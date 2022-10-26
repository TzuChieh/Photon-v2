#include "EditorCore/Thread/GHIThread.h"
#include "RenderCore/NullGHI.h"

#include <Common/logging.h>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(GHIThread, EditorCore);

GHIThread::GHIThread()
	: Base()
	, m_GHI(nullptr)
	, m_nullGHI(std::make_unique<NullGHI>())
{
	PH_LOG(GHIThread, "thread created");
}

GHIThread::~GHIThread()
{
	// Should already been unset
	PH_ASSERT(!m_GHI);

	PH_LOG(GHIThread, "thread destroyed");
}

void GHIThread::onAsyncWorkerStart()
{
	setGHI(m_nullGHI.get());
}

void GHIThread::onAsyncWorkerStop()
{
	setGHI(nullptr);
}

void GHIThread::onAsyncProcessWork(const Work& work)
{
	PH_ASSERT(m_GHI);

	work(*m_GHI);
}

void GHIThread::onBeginFrame()
{
	// TODO
}

void GHIThread::onEndFrame()
{
	// TODO
}

void GHIThread::addSetGHIWork(GHI* const inGHI)
{
	addWork(
		[this, inGHI](GHI& /* ghi */)
		{
			setGHI(inGHI);
		});
}

void GHIThread::setGHI(GHI* const inGHI)
{
	// Nothing to do if the GHIs are the same
	if(m_GHI == inGHI)
	{
		return;
	}

	if(m_GHI)
	{
		m_GHI->unload();
	}
	
	m_GHI = inGHI;

	if(m_GHI)
	{
		m_GHI->load();
	}
}

}// end namespace ph::editor
