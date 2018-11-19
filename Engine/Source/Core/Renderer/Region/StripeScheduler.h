#pragma once

#include "Core/Renderer/Region/WorkScheduler.h"

#include <algorithm>

namespace ph
{

class StripeScheduler : public WorkScheduler
{
public:
	StripeScheduler();
	StripeScheduler(std::size_t numWorkers, const WorkVolume& totalWorkVolume);

private:
	std::size_t m_remainingWidth;
	std::size_t m_perWorkerWidth;

	bool scheduleOne(WorkVolume* out_workVolume) override;
};

// In-header Implementations:

inline StripeScheduler::StripeScheduler() :
	WorkScheduler()
{}

inline StripeScheduler::StripeScheduler(const std::size_t numWorkers, const WorkVolume& totalWorkVolume) :
	WorkScheduler(numWorkers, totalWorkVolume)
{
	m_remainingWidth = std::max(totalWorkVolume.getWorkArea().getWidth(), 
	                            totalWorkVolume.getWorkArea().getHeight());
	m_perWorkerWidth = std::max(m_remainingWidth / numWorkers, std::size_t(1));
}

inline bool StripeScheduler::scheduleOne(WorkVolume* const out_workVolume)
{
	/*if(m_remainingWidth == 0)
	{
		return false;
	}

	if(m_remainingWidth >= m_perWorkerWidth)
	{
		TAABB2D<std::size_t> min = m_totalWorkVolume.getWorkArea().maxVertex;
		*out_workVolume = WorkVolume(m_totalWorkVolume.getWorkArea(), depthPerWorker);
		m_remainingDepth -= depthPerWorker;
		return true;
	}
	else
	{
		*out_workVolume = WorkVolume(m_totalWorkVolume.getWorkArea(), m_remainingDepth);
		m_remainingDepth = 0;
		return true;
	}*/

	return false;
}

}// end namespace ph
