#pragma once

#include "Core/Renderer/Region/WorkUnit.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"

#include <vector>

namespace ph
{

class WorkScheduler
{
public:
	WorkScheduler();
	WorkScheduler(std::size_t numWorkers, const WorkUnit& totalWorkUnit);
	virtual ~WorkScheduler() = default;

	bool schedule(WorkUnit* out_workUnit);
	void scheduleAll(std::vector<WorkUnit>& out_workUnits);
	void submit(const WorkUnit& workUnit);

	float getScheduledFraction() const;
	float getSubmittedFraction() const;

protected:
	std::size_t m_numWorkers;
	WorkUnit    m_totalWorkUnit;
	std::size_t m_totalVolume;
	std::size_t m_scheduledVolume;
	std::size_t m_submittedVolume;

private:
	virtual bool scheduleOne(WorkUnit* out_workUnit) = 0;
};

// In-header Implementations:

inline WorkScheduler::WorkScheduler() : 
	WorkScheduler(1, WorkUnit())
{}

inline WorkScheduler::WorkScheduler(const std::size_t numWorkers, const WorkUnit& totalWorkUnit) :
	m_numWorkers(numWorkers),
	m_totalWorkUnit(totalWorkUnit),
	m_totalVolume(totalWorkUnit.getVolume()),
	m_scheduledVolume(0),
	m_submittedVolume(0)
{}

inline bool WorkScheduler::schedule(WorkUnit* const out_workUnit)
{
	PH_ASSERT(out_workUnit);

	if(scheduleOne(out_workUnit))
	{
		m_scheduledVolume += out_workUnit->getVolume();
		return true;
	}
	else
	{
		return false;
	}
}

inline void WorkScheduler::scheduleAll(std::vector<WorkUnit>& out_workUnits)
{
	WorkUnit workUnit;
	while(schedule(&workUnit))
	{
		out_workUnits.push_back(workUnit);
	}
}

inline void WorkScheduler::submit(const WorkUnit& workUnit)
{
	m_submittedVolume += workUnit.getVolume();
}

inline float WorkScheduler::getScheduledFraction() const
{
	return static_cast<float>(m_scheduledVolume) / static_cast<float>(m_totalVolume);
}

inline float WorkScheduler::getSubmittedFraction() const
{
	return static_cast<float>(m_submittedVolume) / static_cast<float>(m_totalVolume);
}

}// end namespace ph