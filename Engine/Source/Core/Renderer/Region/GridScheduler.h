#pragma once

#include "Core/Renderer/Region/WorkScheduler.h"
#include "Math/math.h"
#include "Math/constant.h"
#include "Common/assertion.h"

#include <algorithm>
#include <cmath>

namespace ph
{

/*
	Slice work region into rectangular cells with similar dimensions, each 
	region has complete depth.
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
		std::size_t             numWorkers,
		const WorkUnit&         totalWorkUnit);

	GridScheduler(
		std::size_t             numWorkers,
		const WorkUnit&         totalWorkUnit,
		const Vector2S&         numCells);

	GridScheduler(
		std::size_t             numWorkers, 
		const WorkUnit&         totalWorkUnit,
		const Vector2S&         numCells,
		EOrigin                 origin,
		constant::AxisIndexType prioriAxis);

private:
	Vector2S                m_numCells;
	EOrigin                 m_origin;
	constant::AxisIndexType m_prioriAxis;
	Vector2S                m_currentCell;

	void scheduleOne(WorkUnit* out_workUnit) override;
};

// In-header Implementations:

inline GridScheduler::GridScheduler() :
	WorkScheduler()
{}

inline GridScheduler::GridScheduler(
	const std::size_t numWorkers,
	const WorkUnit&   totalWorkUnit) : 

	GridScheduler(
		numWorkers, 
		totalWorkUnit,
		Vector2S(
			static_cast<std::size_t>(std::ceil(math::fast_sqrt(numWorkers * static_cast<float>(totalWorkUnit.getAspectRatio())))),
			static_cast<std::size_t>(std::ceil(math::fast_sqrt(numWorkers / static_cast<float>(totalWorkUnit.getAspectRatio()))))))
{}

inline GridScheduler::GridScheduler(
	const std::size_t numWorkers,
	const WorkUnit&   totalWorkUnit,
	const Vector2S&   numCells) :

	GridScheduler(
		numWorkers, 
		totalWorkUnit, 
		numCells,
		EOrigin::LOWER_LEFT,
		constant::X_AXIS)
{}

inline GridScheduler::GridScheduler(
	const std::size_t             numWorkers,
	const WorkUnit&               totalWorkUnit,
	const Vector2S&               numCells,
	const EOrigin                 origin,
	const constant::AxisIndexType prioriAxis) :

	WorkScheduler(numWorkers, totalWorkUnit),

	m_numCells   (numCells),
	m_origin     (origin),
	m_prioriAxis (prioriAxis),
	m_currentCell(0, 0)
{}

inline void GridScheduler::scheduleOne(WorkUnit* const out_workUnit)
{
	PH_ASSERT(out_workUnit);

	// Cell coordinates are always in the canonical Cartesian space. Mapping
	// only performed on divided ranges.

	if(m_currentCell.x < m_numCells.x && m_currentCell.y < m_numCells.y)
	{
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

		if(m_prioriAxis == constant::X_AXIS)
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
	}
	else
	{
		*out_workUnit = WorkUnit();
	}
}

}// end namespace ph
