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
	const std::size_t depthPerRegion) : 

	m_pendingRegions    (),
	m_fullRegion        (fullRegion)
{
	m_terminateThreshold = precisionStandard * 0.0002_r;
	m_splitThreshold     = 256.0_r * m_terminateThreshold;
	m_depthPerRegion     = depthPerRegion;

	// divide the full region initially to facilitate parallelism

	GridScheduler initialScheduler(numWorkers, WorkUnit(fullRegion, depthPerRegion));

	WorkUnit workUnit;
	while(initialScheduler.schedule(&workUnit))
	{
		m_pendingRegions.push(workUnit.getRegion());
	}
}

bool DammertzDispatcher::dispatch(WorkUnit* const out_workUnit)
{
	PH_ASSERT(out_workUnit);

	if(m_pendingRegions.empty())
	{
		return false;
	}

	std::cerr << "# regions = " << m_pendingRegions.size() << std::endl;

	const Region newRegion = m_pendingRegions.front();
	m_pendingRegions.pop();

	*out_workUnit = WorkUnit(newRegion, m_depthPerRegion);
	return true;
}

}// end namespace ph