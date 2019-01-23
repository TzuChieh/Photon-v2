#pragma once

#include "Core/Renderer/Region/WorkScheduler.h"
#include "Math/math.h"
#include "Common/assertion.h"
#include "Math/TVector2.h"

#include <algorithm>
#include <cmath>

namespace ph
{

/*
	Divide work region into squares, each square has the complete depth. 
	The order of each scheduled square forms a spiral shape originating
	from the center.
*/
//class SpiralGridScheduler : public WorkScheduler
//{
//public:
//	SpiralGridScheduler();
//	SpiralGridScheduler(
//		std::size_t       numWorkers, 
//		const WorkVolume& totalWorkVolume,
//		std::size_t       squareSize);
//
//private:
//	int64           m_squareSize;
//	int64           m_headSizeInSquares;
//	TVector2<int64> m_headPosInSquares;
//	TVector2<int64> m_turnPos;
//	int             m_turnIndex;
//	int64           m_numCycles;
//
//	bool scheduleOne(WorkVolume* out_workVolume) override;
//
//	Region calcCurrentRegion() const;
//};
//
//// In-header Implementations:
//
//inline SpiralGridScheduler::SpiralGridScheduler() :
//	WorkScheduler()
//{}
//
//inline SpiralGridScheduler::SpiralGridScheduler(
//	const std::size_t numWorkers,
//	const WorkVolume& totalWorkVolume,
//	const std::size_t squareSize) :
//	WorkScheduler(numWorkers, totalWorkVolume)
//{
//	m_squareSize = static_cast<int64>(squareSize);
//
//	m_headSizeInSquares = static_cast<int64>(std::sqrt(numWorkers));
//	PH_ASSERT_GT(m_headSizeInSquares, 0);
//
//	m_headPosInSquares = {0, 0};
//
//	m_turnPos = totalWorkVolume.getRegion().calcCenter().sub(m_headSizeInSquares * m_squareSize / 2);
//	m_turnIndex = 0;
//	m_numCycles = 0;
//}
//
//inline bool SpiralGridScheduler::scheduleOne(WorkVolume* const out_workVolume)
//{
//	PH_ASSERT(out_workVolume);
//
//	Region region(m_headPos, m_headPos.add(TVector2<int64>(m_inHeadPos * m_squareSize)));
//	region.intersectWith(m_totalWorkVolume.getRegion());
//
//	if(m_headPos.equals({m_headSize - 1, m_headSize - 1}))
//	{
//
//	}
//
//	if(m_numScheduled < m_numWorkers)
//	{
//		PH_ASSERT(out_workVolume);
//
//		const auto sideRange = math::ith_evenly_divided_range(m_numScheduled, m_sideLength, m_numWorkers);
//
//		Region stripRegion = m_totalWorkVolume.getRegion();
//		stripRegion.minVertex[m_slicedAxis] += static_cast<int64>(sideRange.first);
//		stripRegion.maxVertex[m_slicedAxis] -= static_cast<int64>(m_sideLength - sideRange.second);
//
//		*out_workVolume = WorkVolume(stripRegion, m_totalWorkVolume.getDepth());
//
//		++m_numScheduled;
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}
//
//inline Region SpiralGridScheduler::calcCurrentRegion() const
//{
//	int64           m_squareSize;
//	int64           m_headSizeInSquares;
//	TVector2<int64> m_headPosInSquares;
//	TVector2<int64> m_turnPos;
//	int             m_turnIndex;
//	int64           m_numCycles;
//
//	if(m_turnIndex != 0)
//	{
//		PH_ASSERT_IN_RANGE_INCLUSIVE(m_turnIndex, 1, 4);
//
//
//	}
//	else
//	{
//		return Region(m_turnPos, m_turnPos);
//	}
//}

}// end namespace ph
