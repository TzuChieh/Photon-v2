#include "RenderCore/GHIThread.h"
#include "RenderCore/NullGHI.h"

#include <Common/logging.h>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(GHIThread, RenderCore);

GHIThread::GHIThread()
	: Base()
	, m_GHI(nullptr)
	, m_nullGHI(std::make_unique<NullGHI>())
	, m_frameTimer()
	, m_frameTimeMs(0)
{}

GHIThread::~GHIThread()
{
	// Should already been unset
	PH_ASSERT(!m_GHI);
}

void GHIThread::onAsyncWorkerStart()
{
	PH_LOG(GHIThread, "thread started");

	setGHI(m_nullGHI.get());
}

void GHIThread::onAsyncWorkerStop()
{
	setGHI(nullptr);

	PH_LOG(GHIThread, "thread stopped");
}

void GHIThread::onAsyncProcessWork(const Work& work)
{
	PH_ASSERT(m_GHI);

	work(*m_GHI);
}

void GHIThread::onBeginFrame()
{
	addWork(
		[this](GHI& /* ghi */)
		{
			m_frameTimer.start();
		});
}

void GHIThread::onEndFrame()
{
	// Swap buffer at the end of end frame
	addWork(
		[](GHI& ghi)
		{
			ghi.swapBuffers();
		});

	addWork(
		[this](GHI& /* ghi */)
		{
			m_frameTimer.stop();
			m_frameTimeMs.store(m_frameTimer.getDeltaMs<float32>(), std::memory_order_relaxed);
		});
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
