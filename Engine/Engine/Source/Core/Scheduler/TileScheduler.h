#pragma once

#include "Core/Scheduler/WorkScheduler.h"
#include "Math/math.h"
#include "Math/constant.h"
#include "Core/Scheduler/GridScheduler.h"

#include <Common/assertion.h>

#include <algorithm>

namespace ph
{

/*!
Divide work region into rectangles with prescribed dimensions.
*/
class TileScheduler : public WorkScheduler
{
public:
	enum class EOrigin
	{
		LowerLeft,
		LowerRight,
		UpperLeft,
		UpperRight
	};

	TileScheduler();

	TileScheduler(
		std::size_t           numWorkers,
		const WorkUnit&       totalWorkUnit,
		const math::Vector2S& tileSize);

	TileScheduler(
		std::size_t           numWorkers, 
		const WorkUnit&       totalWorkUnit,
		const math::Vector2S& tileSize,
		EOrigin               origin,
		std::size_t           prioriAxis);

private:
	GridScheduler m_grid;

	void scheduleOne(WorkUnit* out_workUnit) override;

	constexpr static GridScheduler::EOrigin toGridOrigin(EOrigin origin);
};

// In-header Implementations:

inline TileScheduler::TileScheduler()
	: WorkScheduler()
	, m_grid()
{}

inline TileScheduler::TileScheduler(
	const std::size_t     numWorkers,
	const WorkUnit&       totalWorkUnit,
	const math::Vector2S& tileSize) :

	TileScheduler(
		numWorkers, 
		totalWorkUnit, 
		tileSize,
		EOrigin::LowerLeft,
		math::constant::X_AXIS)// default to X-first as it is likely more cache friendly
{}

inline TileScheduler::TileScheduler(
	const std::size_t     numWorkers,
	const WorkUnit&       totalWorkUnit,
	const math::Vector2S& tileSize,
	const EOrigin         origin,
	const std::size_t     prioriAxis)

	: WorkScheduler(numWorkers, totalWorkUnit)

	, m_grid()
{
	PH_ASSERT_MSG(tileSize.product() > 0, tileSize.toString());

	// Since grid scheduler slices work region evenly, we need to round up the 
	// grid size to make each slice of size `tileSize`
	math::TVector2<int64> gridSize(
		math::ceil_div(totalWorkUnit.getWidth(),  static_cast<int64>(tileSize.x())),
		math::ceil_div(totalWorkUnit.getHeight(), static_cast<int64>(tileSize.y())));
	gridSize.mulLocal(math::TVector2<int64>(tileSize));

	const Region gridWorkRegion(
		totalWorkUnit.getRegion().getMinVertex(),
		totalWorkUnit.getRegion().getMinVertex() + gridSize);

	m_grid = GridScheduler(
		numWorkers,
		WorkUnit(gridWorkRegion, totalWorkUnit.getDepth()),
		math::Vector2S(gridWorkRegion.getExtents()) / tileSize,
		toGridOrigin(origin),
		prioriAxis);
}

inline void TileScheduler::scheduleOne(WorkUnit* const out_workUnit)
{
	PH_ASSERT(out_workUnit);

	// Tile coordinates are always in the canonical Cartesian space, mapping
	// is performed later on.

	WorkUnit gridWorkUnit;
	if(m_grid.schedule(&gridWorkUnit))
	{
		Region tileWorkRegion = gridWorkUnit.getRegion();
		tileWorkRegion.intersectWith(m_totalWorkUnit.getRegion());

		if(!tileWorkRegion.isEmpty())
		{
			*out_workUnit = WorkUnit(tileWorkRegion, gridWorkUnit.getDepth());
			return;
		}
	}

	*out_workUnit = WorkUnit();
}

inline constexpr GridScheduler::EOrigin TileScheduler::toGridOrigin(const EOrigin origin)
{
	switch(origin)
	{
	case EOrigin::LowerLeft:  return GridScheduler::EOrigin::LowerLeft;
	case EOrigin::LowerRight: return GridScheduler::EOrigin::LowerRight;
	case EOrigin::UpperLeft:  return GridScheduler::EOrigin::UpperLeft;
	case EOrigin::UpperRight: return GridScheduler::EOrigin::UpperRight;

	default: return GridScheduler::EOrigin::LowerLeft;
	}
}

}// end namespace ph
