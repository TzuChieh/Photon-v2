#pragma once

#include "Core/Renderer/Region/IWorkDispatcher.h"
#include "Core/Renderer/Region/WorkUnit.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"

#include <vector>

namespace ph
{

/*! @brief A manager that distributes a fixed amount of work to workers.

The scheduler will deliver some amount of work on demand, where the total 
amount of work is fixed. The sizes of each delivered work is implementation 
defined.
*/
class WorkScheduler : public IWorkDispatcher
{
public:
	/*! @brief A scheduler with no work load.
	*/
	WorkScheduler();

	/*! @brief A scheduler with specific number of workers and work load.
	*/
	WorkScheduler(std::size_t numWorkers, const WorkUnit& totalWorkUnit);

	bool dispatch(WorkUnit* out_workUnit) override;

	/*! @brief Get some amount of work.

	@return True when non-zero amount of work has been scheduled; otherwise, no
	work has been scheduled and no more is yet to be scheduled (the scheduler 
	is exhausted).
	*/
	bool schedule(WorkUnit* out_workUnit);

	void scheduleAll(std::vector<WorkUnit>& out_workUnits);
	void submit(const WorkUnit& workUnit);

	float getScheduledFraction() const;
	float getSubmittedFraction() const;

	/*! @brief Checks if all works had been scheduled.

	@return True if there is no more work.
	*/
	bool isExhausted() const;

protected:
	std::size_t m_numWorkers;
	WorkUnit    m_totalWorkUnit;
	std::size_t m_totalVolume;
	std::size_t m_scheduledVolume;
	std::size_t m_submittedVolume;

private:
	virtual void scheduleOne(WorkUnit* out_workUnit) = 0;
};

// In-header Implementations:

inline WorkScheduler::WorkScheduler() : 
	WorkScheduler(1, WorkUnit())
{}

inline WorkScheduler::WorkScheduler(const std::size_t numWorkers, const WorkUnit& totalWorkUnit) :
	m_numWorkers     (numWorkers),
	m_totalWorkUnit  (totalWorkUnit),
	m_totalVolume    (totalWorkUnit.getVolume()),
	m_scheduledVolume(0),
	m_submittedVolume(0)
{
	PH_ASSERT_GE(numWorkers, 1);
}

inline bool WorkScheduler::dispatch(WorkUnit* const out_workUnit)
{
	return schedule(out_workUnit);
}

inline bool WorkScheduler::schedule(WorkUnit* const out_workUnit)
{
	PH_ASSERT(out_workUnit);

	while(!isExhausted())
	{
		scheduleOne(out_workUnit);

		const std::size_t volume = out_workUnit->getVolume();
		if(volume > 0)
		{
			m_scheduledVolume += volume;
			return true;
		}
	}

	PH_ASSERT_EQ(m_scheduledVolume, m_totalVolume);
	return false;
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

	PH_ASSERT_LE(m_submittedVolume, m_totalVolume);
}

inline float WorkScheduler::getScheduledFraction() const
{
	return static_cast<float>(m_scheduledVolume) / static_cast<float>(m_totalVolume);
}

inline float WorkScheduler::getSubmittedFraction() const
{
	return static_cast<float>(m_submittedVolume) / static_cast<float>(m_totalVolume);
}

inline bool WorkScheduler::isExhausted() const
{
	return m_scheduledVolume == m_totalVolume;
}

}// end namespace ph
