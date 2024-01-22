#pragma once

#include "Core/Scheduler/SpiralScheduler.h"
#include "Core/Scheduler/GridScheduler.h"
#include "Math/math.h"

#include <Common/assertion.h>

#include <algorithm>
#include <cmath>

namespace ph
{

/*!
A spiral scheduler that further schedules each work unit using
a grid scheduler.
*/
class SpiralGridScheduler : public WorkScheduler
{
public:
	SpiralGridScheduler();

	SpiralGridScheduler(
		std::size_t           numWorkers,
		const WorkUnit&       totalWorkUnit,
		const math::Vector2S& spiralTileSize);

	SpiralGridScheduler(
		std::size_t           numWorkers,
		const WorkUnit&       totalWorkUnit,
		const math::Vector2S& spiralTileSize,
		const math::Vector2S& numGridCells);

private:
	SpiralScheduler m_spiralScheduler;
	math::Vector2S  m_numGridCells;
	GridScheduler   m_currentGrid;

	void scheduleOne(WorkUnit* out_workUnit) override;
};

// In-header Implementations:

inline SpiralGridScheduler::SpiralGridScheduler()

	: WorkScheduler()

	, m_spiralScheduler()
	, m_numGridCells   (0)
	, m_currentGrid    ()
{}

inline SpiralGridScheduler::SpiralGridScheduler(
	const std::size_t     numWorkers,
	const WorkUnit&       totalWorkUnit,
	const math::Vector2S& spiralTileSize) :

	SpiralGridScheduler(
		numWorkers, 
		totalWorkUnit,
		spiralTileSize,
		math::Vector2S(static_cast<std::size_t>(
			std::max(math::fast_sqrt(static_cast<float>(numWorkers)), 1.0f))))
{}

inline SpiralGridScheduler::SpiralGridScheduler(
	const std::size_t     numWorkers,
	const WorkUnit&       totalWorkUnit,
	const math::Vector2S& spiralTileSize,
	const math::Vector2S& numGridCells) :

	WorkScheduler(numWorkers, totalWorkUnit),

	m_spiralScheduler(numWorkers, totalWorkUnit, spiralTileSize),
	m_numGridCells   (numGridCells),
	m_currentGrid    ()
{}

inline void SpiralGridScheduler::scheduleOne(WorkUnit* const out_workUnit)
{
	PH_ASSERT(out_workUnit);

	if(!m_currentGrid.schedule(out_workUnit))
	{
		WorkUnit parentWorkUnit;
		if(m_spiralScheduler.schedule(&parentWorkUnit))
		{
			m_currentGrid = GridScheduler(
				m_numGridCells.product(), 
				parentWorkUnit, 
				m_numGridCells);

			m_currentGrid.schedule(out_workUnit);
			PH_ASSERT_GT(out_workUnit->getVolume(), 0);
		}
		else
		{
			*out_workUnit = WorkUnit();
		}
	}
}

}// end namespace ph
