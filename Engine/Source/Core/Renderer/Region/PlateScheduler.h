#pragma once

#include "Core/Renderer/Region/WorkScheduler.h"
#include "Common/assertion.h"

#include <algorithm>

namespace ph
{

class PlateScheduler : public WorkScheduler
{
public:
	PlateScheduler();
	PlateScheduler(std::size_t numWorkers, const WorkVolume& totalWorkVolume);

private:
	std::size_t m_remainingDepth;

	bool scheduleOne(WorkVolume* out_workVolume) override;
};

// In-header Implementations:

inline PlateScheduler::PlateScheduler() : 
	WorkScheduler()
{}

inline PlateScheduler::PlateScheduler(const std::size_t numWorkers, const WorkVolume& totalWorkVolume) : 
	WorkScheduler(numWorkers, totalWorkVolume),
	m_remainingDepth(totalWorkVolume.getWorkDepth())
{}

inline bool PlateScheduler::scheduleOne(WorkVolume* const out_workVolume)
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