#pragma once

#include "Core/Renderer/Region/WorkVolume.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"

#include <vector>

namespace ph
{

class WorkScheduler
{
public:
	WorkScheduler();
	WorkScheduler(std::size_t numWorkers, const WorkVolume& totalWorkVolume);
	virtual ~WorkScheduler() = default;

	bool schedule(WorkVolume* out_workVolume);
	void scheduleAll(std::vector<WorkVolume>& out_workVolumes);
	void submit(const WorkVolume& workVolume);

	float getScheduledFraction() const;
	float getSubmittedFraction() const;

protected:
	std::size_t m_numWorkers;
	WorkVolume m_totalWorkVolume;
	std::size_t m_totalVolume;
	std::size_t m_scheduledVolume;
	std::size_t m_submittedVolume;

private:
	virtual bool scheduleOne(WorkVolume* out_workVolume) = 0;
};

// In-header Implementations:

inline WorkScheduler::WorkScheduler() : 
	WorkScheduler(1, WorkVolume())
{}

inline WorkScheduler::WorkScheduler(const std::size_t numWorkers, const WorkVolume& totalWorkVolume) : 
	m_numWorkers(numWorkers),
	m_totalWorkVolume(totalWorkVolume),
	m_totalVolume(totalWorkVolume.getWorkArea().calcArea() * totalWorkVolume.getWorkDepth()),
	m_scheduledVolume(0),
	m_submittedVolume(0)
{}

inline bool WorkScheduler::schedule(WorkVolume* const out_workVolume)
{
	PH_ASSERT(out_workVolume);

	if(scheduleOne(out_workVolume))
	{
		m_scheduledVolume += out_workVolume->getWorkArea().calcArea() * out_workVolume->getWorkDepth();
		return true;
	}
	else
	{
		return false;
	}
}

inline void WorkScheduler::scheduleAll(std::vector<WorkVolume>& out_workVolumes)
{
	WorkVolume workVolume;
	while(schedule(&workVolume))
	{
		out_workVolumes.push_back(workVolume);
	}
}

inline void WorkScheduler::submit(const WorkVolume& workVolume)
{
	m_submittedVolume += workVolume.getWorkArea().calcArea() * workVolume.getWorkDepth();
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