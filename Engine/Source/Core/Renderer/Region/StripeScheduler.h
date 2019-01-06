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
		std::size_t       numWorkers, 
		const WorkVolume& totalWorkVolume,
		int               slicedAxis);

private:
	int         m_slicedAxis;
	std::size_t m_numScheduled;
	std::size_t m_sideLength;

	bool scheduleOne(WorkVolume* out_workVolume) override;
};

// In-header Implementations:

inline StripeScheduler::StripeScheduler() :
	WorkScheduler()
{}

inline StripeScheduler::StripeScheduler(
	const std::size_t numWorkers,
	const WorkVolume& totalWorkVolume,
	const int         slicedAxis) :

	WorkScheduler(numWorkers, totalWorkVolume),

	m_slicedAxis(slicedAxis),
	m_numScheduled(0),
	m_sideLength(static_cast<std::size_t>(m_totalWorkVolume.getRegion().getExtents()[slicedAxis]))
{}

inline bool StripeScheduler::scheduleOne(WorkVolume* const out_workVolume)
{
	if(m_numScheduled < m_numWorkers)
	{
		PH_ASSERT(out_workVolume);

		const auto sideRange = math::ith_evenly_divided_range(m_numScheduled, m_sideLength, m_numWorkers);

		Region stripRegion = m_totalWorkVolume.getRegion();
		stripRegion.minVertex[m_slicedAxis] += static_cast<int64>(sideRange.first);
		stripRegion.maxVertex[m_slicedAxis] -= static_cast<int64>(m_sideLength - sideRange.second);

		*out_workVolume = WorkVolume(stripRegion, m_totalWorkVolume.getDepth());

		++m_numScheduled;
		return true;
	}
	else
	{
		return false;
	}
}

}// end namespace ph
