#pragma once

#include "Core/Renderer/Region/WorkScheduler.h"
#include "Math/math.h"
#include "Common/assertion.h"

#include <algorithm>

namespace ph
{

/*
	Divide work region into stripes, each stripe has the complete depth. This
	scheduler is essentially a plate scheduler, just in different dimensions.
*/
class StripeScheduler : public WorkScheduler
{
public:
	StripeScheduler();
	StripeScheduler(
		std::size_t     numWorkers, 
		const WorkUnit& totalWorkUnit,
		int             slicedAxis);

private:
	int         m_slicedAxis;
	std::size_t m_numScheduled;
	std::size_t m_sideLength;

	void scheduleOne(WorkUnit* out_workUnit) override;
};

// In-header Implementations:

inline StripeScheduler::StripeScheduler() :
	WorkScheduler()
{}

inline StripeScheduler::StripeScheduler(
	const std::size_t numWorkers,
	const WorkUnit&   totalWorkUnit,
	const int         slicedAxis) :

	WorkScheduler(numWorkers, totalWorkUnit),

	m_slicedAxis  (slicedAxis),
	m_numScheduled(0),
	m_sideLength  (static_cast<std::size_t>(m_totalWorkUnit.getRegion().getExtents()[slicedAxis]))
{}

inline void StripeScheduler::scheduleOne(WorkUnit* const out_workUnit)
{
	PH_ASSERT(out_workUnit);

	if(m_numScheduled < m_numWorkers)
	{
		const auto sideRange = math::ith_evenly_divided_range(m_numScheduled, m_sideLength, m_numWorkers);

		Region stripRegion = m_totalWorkUnit.getRegion();
		stripRegion.minVertex[m_slicedAxis] += static_cast<int64>(sideRange.first);
		stripRegion.maxVertex[m_slicedAxis] -= static_cast<int64>(m_sideLength - sideRange.second);

		*out_workUnit = WorkUnit(stripRegion, m_totalWorkUnit.getDepth());

		++m_numScheduled;
	}
	else
	{
		*out_workUnit = WorkUnit();
	}
}

}// end namespace ph
