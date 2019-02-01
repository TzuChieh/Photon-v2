#pragma once

#include "Core/Renderer/Region/WorkScheduler.h"
#include "Math/math.h"
#include "Common/assertion.h"
#include "Core/Renderer/Region/GridScheduler.h"

#include <algorithm>

namespace ph
{

/*
	Divide work region into rectangles with prescribed dimensions.
*/
class TileScheduler : public WorkScheduler
{
public:
	enum class EOrigin
	{
		LOWER_LEFT,
		LOWER_RIGHT,
		UPPER_LEFT,
		UPPER_RIGHT
	};

	TileScheduler();

	TileScheduler(
		std::size_t     numWorkers,
		const WorkUnit& totalWorkUnit,
		const Vector2S& tileSize);

	TileScheduler(
		std::size_t     numWorkers, 
		const WorkUnit& totalWorkUnit,
		const Vector2S& tileSize,
		EOrigin         origin,
		int             prioriAxis);

private:
	GridScheduler m_grid;

	void scheduleOne(WorkUnit* out_workUnit) override;

	constexpr static GridScheduler::EOrigin toGridOrigin(EOrigin origin);
};

// In-header Implementations:

inline TileScheduler::TileScheduler() :
	WorkScheduler()
{}

inline TileScheduler::TileScheduler(
	const std::size_t numWorkers,
	const WorkUnit&   totalWorkUnit,
	const Vector2S&   tileSize) :

	TileScheduler(
		numWorkers, 
		totalWorkUnit, 
		tileSize,
		EOrigin::LOWER_LEFT,
		math::X_AXIS)
{}

inline TileScheduler::TileScheduler(
	std::size_t     numWorkers,
	const WorkUnit& totalWorkUnit,
	const Vector2S& tileSize,
	const EOrigin   origin,
	const int       prioriAxis) : 

	WorkScheduler(numWorkers, totalWorkUnit),

	m_grid()
{
	PH_ASSERT_MSG(tileSize.product() > 0, tileSize.toString());

	// since grid scheduler slices work region, we need to round up the 
	// grid size to make each slice with size <tileSize>
	TVector2<int64> gridSize(
		math::ceil_div_positive(totalWorkUnit.getWidth(),  static_cast<int64>(tileSize.x)),
		math::ceil_div_positive(totalWorkUnit.getHeight(), static_cast<int64>(tileSize.y)));
	gridSize.mulLocal(TVector2<int64>(tileSize));

	const Region gridWorkRegion(
		totalWorkUnit.getRegion().minVertex, 
		totalWorkUnit.getRegion().minVertex + gridSize);

	m_grid = GridScheduler(
		numWorkers,
		WorkUnit(gridWorkRegion, totalWorkUnit.getDepth()),
		Vector2S(gridWorkRegion.getExtents()) / tileSize,
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

		if(tileWorkRegion.isValid())
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
	case EOrigin::LOWER_LEFT:  return GridScheduler::EOrigin::LOWER_LEFT;
	case EOrigin::LOWER_RIGHT: return GridScheduler::EOrigin::LOWER_RIGHT;
	case EOrigin::UPPER_LEFT:  return GridScheduler::EOrigin::UPPER_LEFT;
	case EOrigin::UPPER_RIGHT: return GridScheduler::EOrigin::UPPER_RIGHT;

	default: return GridScheduler::EOrigin::LOWER_LEFT;
	}
}

}// end namespace ph
