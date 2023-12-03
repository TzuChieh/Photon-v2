#pragma once

#include "Core/Scheduler/WorkScheduler.h"
#include "Common/assertion.h"
#include "Math/math.h"

#include <algorithm>

namespace ph
{

/*
	Schedules a complete region, without any planar division, to each worker. 
	Only work depth is distributed among workers.
*/
class PlateScheduler : public WorkScheduler
{
public:
	PlateScheduler();
	PlateScheduler(std::size_t numWorkers, const WorkUnit& totalWorkUnit);

private:
	std::size_t m_numScheduled;

	void scheduleOne(WorkUnit* out_workUnit) override;
};

// In-header Implementations:

inline PlateScheduler::PlateScheduler() : 
	WorkScheduler()
{}

inline PlateScheduler::PlateScheduler(const std::size_t numWorkers, const WorkUnit& totalWorkUnit) :
	WorkScheduler(numWorkers, totalWorkUnit),
	m_numScheduled(0)
{}

inline void PlateScheduler::scheduleOne(WorkUnit* const out_workUnit)
{
	PH_ASSERT(out_workUnit);

	if(m_numScheduled < m_numWorkers)
	{
		const auto depthRange = math::ith_evenly_divided_range(
			m_numScheduled, m_totalWorkUnit.getDepth(), m_numWorkers);
		*out_workUnit = WorkUnit(m_totalWorkUnit.getRegion(), depthRange.second - depthRange.first);

		++m_numScheduled;
	}
	else
	{
		*out_workUnit = WorkUnit();
	}
}

}// end namespace ph
