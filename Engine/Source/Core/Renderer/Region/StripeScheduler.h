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
	bool scheduleOne(WorkVolume* out_workVolume) override;
};

// In-header Implementations:

inline StripeScheduler::StripeScheduler() :
	WorkScheduler()
{}

inline StripeScheduler::StripeScheduler(const std::size_t numWorkers, const WorkVolume& totalWorkVolume) :
	WorkScheduler(numWorkers, totalWorkVolume)
{}

inline bool StripeScheduler::scheduleOne(WorkVolume* const out_workVolume)
{
	if(m_remainingDepth == 0)
	{
		return false;
	}

	const std::size_t depthPerWorker = std::max(m_totalWorkVolume.getWorkDepth() / m_numWorkers, std::size_t(1));
	if(m_remainingDepth >= depthPerWorker)
	{
		*out_workVolume = WorkVolume(m_totalWorkVolume.getWorkArea(), depthPerWorker);
		m_remainingDepth -= depthPerWorker;
		return true;
	}
	else
	{
		*out_workVolume = WorkVolume(m_totalWorkVolume.getWorkArea(), m_remainingDepth);
		m_remainingDepth = 0;
		return true;
	}
}

}// end namespace ph
