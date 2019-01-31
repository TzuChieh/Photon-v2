#include "Core/Renderer/Region/DammertzAdaptiveDispatcher.h"
#include "Common/assertion.h"
#include "Frame/TFrame.h"
#include "Math/math.h"

#include <cmath>
#include <string>
#include <iostream>

namespace ph
{

DammertzAdaptiveDispatcher::DammertzAdaptiveDispatcher(
	const Region& fullRegion) :

	DammertzAdaptiveDispatcher(fullRegion, 1.0_r, 4)
{}

DammertzAdaptiveDispatcher::DammertzAdaptiveDispatcher(
	const Region&     fullRegion, 
	const real        precisionStandard, 
	const std::size_t depthPerRegion) : 

	m_pendingRegions(),
	m_fullRegion(fullRegion),
	m_rcpNumRegionPixels(1.0_r / fullRegion.calcArea())
{
	m_terminateThreshold = precisionStandard * 0.0002_r;
	m_splitThreshold = 256.0_r * m_terminateThreshold;
	m_depthPerRegion = depthPerRegion;

	m_pendingRegions.push(fullRegion);
}

bool DammertzAdaptiveDispatcher::dispatch(WorkUnit* const out_workUnit)
{
	PH_ASSERT(out_workUnit);

	if(m_pendingRegions.empty())
	{
		return false;
	}

	const Region newRegion = m_pendingRegions.front();
	m_pendingRegions.pop();

	*out_workUnit = WorkUnit(newRegion, m_depthPerRegion);
	return true;
}

void DammertzAdaptiveDispatcher::analyzeFinishedRegion(
	const Region& finishedRegion,
	const HdrRgbFrame& allEffortFrame,
	const HdrRgbFrame& halfEffortFrame)
{
	PH_ASSERT_GE(finishedRegion.minVertex.x, 0);
	PH_ASSERT_GE(finishedRegion.minVertex.y, 0);
	PH_ASSERT_LE(finishedRegion.getWidth(),  allEffortFrame.widthPx());
	PH_ASSERT_LE(finishedRegion.getHeight(), allEffortFrame.heightPx());
	PH_ASSERT_LE(finishedRegion.getWidth(),  halfEffortFrame.widthPx());
	PH_ASSERT_LE(finishedRegion.getHeight(), halfEffortFrame.heightPx());

	const TAABB2D<uint32> frameRegion(finishedRegion);

	real regionError = 0.0_r;
	for(uint32 y = frameRegion.minVertex.y; y < frameRegion.maxVertex.y; ++y)
	{
		for(uint32 x = frameRegion.minVertex.x; x < frameRegion.maxVertex.x; ++x)
		{
			HdrRgbFrame::Pixel I, A;
			allEffortFrame.getPixel(x, y, &I);
			halfEffortFrame.getPixel(x, y, &A);

			const real numerator      = I.sub(A).abs().sum();
			const real sumOfI         = I.sum();
			const real rcpDenominator = sumOfI > 0 ? math::fast_rcp_sqrt(sumOfI) : 0;

			regionError += numerator * rcpDenominator;
		}
	}
	regionError /= frameRegion.calcArea();
	regionError *= std::sqrt(frameRegion.calcArea() * m_rcpNumRegionPixels);

	PH_ASSERT_MSG(std::isfinite(regionError), std::to_string(regionError));

	std::cerr << "region = " << frameRegion.toString() << "error = " << regionError << std::endl;

	if(regionError >= m_splitThreshold)
	{
		// error is large, added for more effort
		addPendingRegion(finishedRegion);

		std::cerr << "too large, split = " << m_splitThreshold << std::endl;
	}
	else if(regionError >= m_terminateThreshold)
	{
		// error is small, splitted and added for more effort
		const int maxDimension = finishedRegion.getExtents().maxDimension();
		const int64 midPoint = (finishedRegion.minVertex[maxDimension] + finishedRegion.maxVertex[maxDimension]) / 2;
		const auto splittedRegion = finishedRegion.getSplitted(maxDimension, midPoint);
		addPendingRegion(splittedRegion.first);
		addPendingRegion(splittedRegion.second);

		std::cerr << "small, terminate = " << m_terminateThreshold << std::endl;
	}
	else
	{
		// error is very small, no further effort needed
		// do nothing

		std::cerr << "very small, terminated" << std::endl;
	}
}

}// end namespace ph