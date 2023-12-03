#include "Core/Renderer/RenderWorker.h"
#include "Core/Renderer/RenderWork.h"

#include <iostream>

namespace ph
{

RenderWorker::RenderWorker(const RendererProxy& renderer, 
                           const uint32 id) :
	m_renderer    (renderer),
	m_id          (id),
	m_work        (nullptr),
	m_totalWork   (0),
	m_workDone    (0),
	m_numElapsedMs(0)
{}

RenderWorker::RenderWorker(const RenderWorker& other) : 
	m_renderer    (other.m_renderer),
	m_id          (other.m_id),
	m_work        (other.m_work),
	m_totalWork   (other.m_totalWork.load(std::memory_order_relaxed)),
	m_workDone    (other.m_workDone.load(std::memory_order_relaxed)),
	m_numElapsedMs(other.m_numElapsedMs.load(std::memory_order_relaxed))
{}

void RenderWorker::run()
{
	doWork();
}

void RenderWorker::doWork()
{
	if(!m_work)
	{
		return;
	}

	m_work->work();
}

void RenderWorker::setWork(RenderWork* const work)
{
	m_work = work;
	if(m_work)
	{
		//m_work->setWorker(this);
	}
}

RenderWorker& RenderWorker::operator = (const RenderWorker& rhs)
{
	m_renderer   = rhs.m_renderer;
	m_id         = rhs.m_id;
	m_work       = rhs.m_work;
	m_totalWork  = rhs.m_totalWork.load();
	m_workDone   = rhs.m_workDone.load();

	return *this;
}

}// end namespace ph
