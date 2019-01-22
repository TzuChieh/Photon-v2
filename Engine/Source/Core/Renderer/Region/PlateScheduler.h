#pragma once

#include "Core/Renderer/Region/WorkScheduler.h"
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
	PlateScheduler(std::size_t numWorkers, const WorkVolume& totalWorkVolume);

private:
	std::size_t m_numScheduled;

	bool scheduleOne(WorkVolume* out_workVolume) override;
};

// In-header Implementations:

inline PlateScheduler::PlateScheduler() : 
	WorkScheduler()
{}

inline PlateScheduler::PlateScheduler(const std::size_t numWorkers, const WorkVolume& totalWorkVolume) : 
	WorkScheduler(numWorkers, totalWorkVolume),
	m_numScheduled(0)
{}

inline bool PlateScheduler::scheduleOne(WorkVolume* const out_workVolume)
{
	if(m_numScheduled < m_numWorkers)
	{
		PH_ASSERT(out_workVolume);

		const auto depthRange = math::ith_evenly_divided_range(
			m_numScheduled, m_totalWorkVolume.getDepth(), m_numWorkers);
		*out_workVolume = WorkVolume(m_totalWorkVolume.getRegion(), depthRange.second - depthRange.first);

		++m_numScheduled;
		return true;
	}
	else
	{
		return false;
	}
}

}// end namespace ph