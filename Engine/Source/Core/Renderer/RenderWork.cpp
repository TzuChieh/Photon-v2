#include "Core/Renderer/RenderWork.h"
#include "Common/assertion.h"
#include "Core/Renderer/RenderWorker.h"

namespace ph
{

void RenderWork::setTotalWork(const uint32 totalWork)
{
	PH_ASSERT(m_worker);

	m_worker->setTotalWork(totalWork);
}

void RenderWork::setWorkDone(const uint32 workDone)
{
	PH_ASSERT(m_worker);

	m_worker->setWorkDone(workDone);
}

void RenderWork::incrementWorkDone()
{
	PH_ASSERT(m_worker);

	m_worker->incrementWorkDone();
}

}// end namespace ph