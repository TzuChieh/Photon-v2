#include "Core/Renderer/Region/DammertzDispatcher.h"
#include "Common/assertion.h"
#include "Core/Renderer/Region/GridScheduler.h"



namespace ph
{

DammertzDispatcher::DammertzDispatcher(
	const uint32  numWorkers,
	const Region& fullRegion) :

	DammertzDispatcher(numWorkers, fullRegion, 1.0_r, 16)
{}

DammertzDispatcher::DammertzDispatcher(
	const uint32      numWorkers,
	const Region&     fullRegion, 
	const real        precisionStandard, 
	const std::size_t initialDepthPerRegion,
	const std::size_t minSplittedVolume) :

	m_pendingRegions    (),
	m_fullRegion        (fullRegion)
{
	m_terminateThreshold = precisionStandard * 0.0002_r;
	m_splitThreshold     = 256.0_r * m_terminateThreshold;

	// divide the full region initially to facilitate parallelism

	GridScheduler initialScheduler(
		numWorkers, 
		WorkUnit(fullRegion, std::max(initialDepthPerRegion, std::size_t(1))));

	std::size_t maxWorkVolume = 0;
	WorkUnit workUnit;
	while(initialScheduler.schedule(&workUnit))
	{
		m_pendingRegions.push(workUnit);

		maxWorkVolume = std::max(workUnit.getVolume(), maxWorkVolume);
	}
	PH_ASSERT_GT(maxWorkVolume, 0);

	m_minSplittedVolume = minSplittedVolume != 0 ? minSplittedVolume 
	                                             : maxWorkVolume;
}

bool DammertzDispatcher::dispatch(WorkUnit* const out_workUnit)
{
	PH_ASSERT(out_workUnit);

	if(m_pendingRegions.empty())
	{
		return false;
	}

	const WorkUnit newRegion = m_pendingRegions.front();
	m_pendingRegions.pop();

	*out_workUnit = newRegion;
	return true;
}

}// end namespace ph