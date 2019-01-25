#pragma once

#include "Core/Renderer/Region/WorkScheduler.h"
#include "Math/math.h"
#include "Common/assertion.h"

#include <algorithm>

namespace ph
{

/*
	Slice work region into rectangles, each region has complete depth.
*/
class GridScheduler : public WorkScheduler
{
public:
	enum class EOrigin
	{
		LOWER_LEFT,
		LOWER_RIGHT,
		UPPER_LEFT,
		UPPER_RIGHT
	};

	GridScheduler();

	GridScheduler(
		std::size_t     numWorkers,
		const WorkUnit& totalWorkUnit,
		const Vector2S& numCells);

	GridScheduler(
		std::size_t     numWorkers, 
		const WorkUnit& totalWorkUnit,
		const Vector2S& numCells,
		EOrigin         origin,
		int             prioriAxis);

private:
	Vector2S m_numCells;
	EOrigin  m_origin;
	int      m_prioriAxis;
	Vector2S m_currentCell;

	bool scheduleOne(WorkUnit* out_workUnit) override;
};

// In-header Implementations:

inline GridScheduler::GridScheduler() :
	WorkScheduler()
{}

inline GridScheduler::GridScheduler(
	const std::size_t numWorkers,
	const WorkUnit& totalWorkUnit,
	const Vector2S&   numCells) :

	GridScheduler(
		numWorkers, 
		totalWorkUnit, 
		numCells,
		EOrigin::LOWER_LEFT,
		math::X_AXIS)
{}

inline GridScheduler::GridScheduler(
	std::size_t     numWorkers,
	const WorkUnit& totalWorkUnit,
	const Vector2S& numCells,
	const EOrigin   origin,
	const int       prioriAxis) : 

	WorkScheduler(numWorkers, totalWorkUnit),

	m_numCells(numCells),
	m_origin(origin),
	m_prioriAxis(prioriAxis),
	m_currentCell(0, 0)
{}

inline bool GridScheduler::scheduleOne(WorkUnit* const out_workUnit)
{
	// Cell coordinates are always in the canonical Cartesian space. Mapping
	// only performed on divided ranges.

	if(m_currentCell.x < m_numCells.x && m_currentCell.y < m_numCells.y)
	{
		PH_ASSERT(out_workUnit);

		const std::size_t totalWidth  = m_totalWorkUnit.getRegion().getWidth();
		const std::size_t totalHeight = m_totalWorkUnit.getRegion().getHeight();

		auto sideRangeX = math::ith_evenly_divided_range(
			m_currentCell.x, totalWidth, m_numCells.x);
		if(m_origin == EOrigin::LOWER_RIGHT || m_origin == EOrigin::UPPER_RIGHT)
		{
			const auto size = sideRangeX.second - sideRangeX.first;
			sideRangeX.first  = totalWidth - sideRangeX.second;
			sideRangeX.second = sideRangeX.first + size;
		}

		auto sideRangeY = math::ith_evenly_divided_range(
			m_currentCell.y, totalHeight, m_numCells.y);
		if(m_origin == EOrigin::UPPER_LEFT || m_origin == EOrigin::UPPER_RIGHT)
		{
			const auto size = sideRangeY.second - sideRangeY.first;
			sideRangeY.first  = totalHeight - sideRangeY.second;
			sideRangeY.second = sideRangeY.first + size;
		}

		*out_workUnit = WorkUnit(
			Region(
				TVector2<int64>(Vector2S(sideRangeX.first, sideRangeY.first)) + m_totalWorkUnit.getRegion().minVertex,
				TVector2<int64>(Vector2S(sideRangeX.second, sideRangeY.second)) + m_totalWorkUnit.getRegion().minVertex),
			m_totalWorkUnit.getDepth());

		if(m_prioriAxis == math::X_AXIS)
		{
			++m_currentCell.x;
			if(m_currentCell.x == m_numCells.x)
			{
				m_currentCell.x = 0;
				++m_currentCell.y;
			}
		}
		else
		{
			++m_currentCell.y;
			if(m_currentCell.y == m_numCells.y)
			{
				m_currentCell.y = 0;
				++m_currentCell.x;
			}
		}

		return true;
	}
	else
	{
		return false;
	}
}

}// end namespace ph
