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
	const std::size_t initialDepthPerRegion) : 

	DammertzDispatcher(
		numWorkers,
		fullRegion,
		precisionStandard,
		initialDepthPerRegion,
		16,
		64)
{}

DammertzDispatcher::DammertzDispatcher(
	const uint32      numWorkers,
	const Region&     fullRegion,
	const real        precisionStandard,
	const std::size_t initialDepthPerRegion,
	const std::size_t standardDepthPerRegion,
	const std::size_t terminusDepthPerRegion) :

	m_standardDepthPerRegion(std::max(standardDepthPerRegion, std::size_t(1))),
	m_terminusDepthPerRegion(std::max(terminusDepthPerRegion, std::size_t(1))),
	m_fullRegion            (fullRegion),
	m_pendingRegions        ()
{
	m_terminateThreshold = precisionStandard * 0.0002_r;
	m_splitThreshold     = 256.0_r * m_terminateThreshold;

	PH_ASSERT_GT(initialDepthPerRegion,  0);
	PH_ASSERT_GT(standardDepthPerRegion, 0);
	PH_ASSERT_GT(terminusDepthPerRegion, 0);

	// divide the full region initially to facilitate parallelism

	GridScheduler initialScheduler(
		numWorkers, 
		WorkUnit(fullRegion, std::max(initialDepthPerRegion, std::size_t(1))));

	WorkUnit workUnit;
	while(initialScheduler.schedule(&workUnit))
	{
		m_pendingRegions.push(workUnit);
	}
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