#include "Core/Renderer/Region/DammertzAdaptiveDispatcher.h"
#include "Common/assertion.h"
#include "Frame/TFrame.h"
#include "Math/math.h"

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
	m_fullRegion(fullRegion)
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

	// TODO
}

}// end namespace ph