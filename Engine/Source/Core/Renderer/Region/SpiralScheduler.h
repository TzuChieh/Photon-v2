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
	Divide work region into rectangles, each rectangle has the complete depth. 
	The order of each scheduled rectangle forms a spiral shape originating
	from the center.
*/
class SpiralScheduler : public WorkScheduler
{
public:
	SpiralScheduler();

	SpiralScheduler(
		std::size_t     numWorkers, 
		const WorkUnit& totalWorkUnit,
		std::size_t     squareSize);

	SpiralScheduler(
		std::size_t     numWorkers,
		const WorkUnit& totalWorkUnit,
		const Vector2S& rectangleSize);

private:
	enum class EFacing
	{
		POSITIVE_X,
		POSITIVE_Y,
		NEGATIVE_X,
		NEGATIVE_Y
	};

	TVector2<int64> m_headSize;
	TVector2<int64> m_headPos;
	EFacing         m_headFacing;
	std::size_t     m_currentCycles;
	std::size_t     m_currentSteps;

	void scheduleOne(WorkUnit* out_workUnit) override;
};

// In-header Implementations:

inline SpiralScheduler::SpiralScheduler() :
	WorkScheduler()
{}

inline SpiralScheduler::SpiralScheduler(
	const std::size_t numWorkers,
	const WorkUnit&   totalWorkUnit,
	const std::size_t squareSize) :

	SpiralScheduler(
		numWorkers, 
		totalWorkUnit, 
		Vector2S(squareSize, squareSize))
{}

inline SpiralScheduler::SpiralScheduler(
	const std::size_t numWorkers,
	const WorkUnit&   totalWorkUnit,
	const Vector2S&   rectangleSize) :

	WorkScheduler(numWorkers, totalWorkUnit),

	m_headSize     (rectangleSize),
	m_headPos      (totalWorkUnit.getRegion().calcCenter().sub(m_headSize / 2)),
	m_headFacing   (EFacing::POSITIVE_X),
	m_currentCycles(0),
	m_currentSteps (0)
{}

inline void SpiralScheduler::scheduleOne(WorkUnit* const out_workUnit)
{
	PH_ASSERT(out_workUnit);

	Region headRegion(m_headPos, m_headPos.add(m_headSize));
	headRegion.intersectWith(m_totalWorkUnit.getRegion());
	if(headRegion.isArea())
	{
		*out_workUnit = WorkUnit(headRegion, m_totalWorkUnit.getDepth());
	}
	else
	{
		*out_workUnit = WorkUnit();
	}

	// prepare for next head region

	const std::size_t maxSteps = m_currentCycles * 2;

	++m_currentSteps;
	switch(m_headFacing)
	{
	// advancing to next cycle is only possible in this direction
	case EFacing::POSITIVE_X:
		m_headPos.x += m_headSize.x;
		if(m_currentSteps >= maxSteps)
		{
			m_headFacing = EFacing::POSITIVE_Y;
			m_currentSteps = 0;
			++m_currentCycles;
		}
		break;

	case EFacing::POSITIVE_Y:
		if(m_currentSteps < maxSteps)
		{
			m_headPos.y += m_headSize.y;
		}
		else
		{
			m_headPos.x -= m_headSize.x;
			m_headFacing = EFacing::NEGATIVE_X;
			m_currentSteps = 0;
		}
		break;

	case EFacing::NEGATIVE_X:
		if(m_currentSteps < maxSteps)
		{
			m_headPos.x -= m_headSize.x;
		}
		else
		{
			m_headPos.y -= m_headSize.y;
			m_headFacing = EFacing::NEGATIVE_Y;
			m_currentSteps = 0;
		}
		break;

	case EFacing::NEGATIVE_Y:
		if(m_currentSteps < maxSteps)
		{
			m_headPos.y -= m_headSize.y;
		}
		else
		{
			m_headPos.x += m_headSize.x;
			m_headFacing = EFacing::POSITIVE_X;
			m_currentSteps = 0;
		}
		break;
	}
}

}// end namespace ph
