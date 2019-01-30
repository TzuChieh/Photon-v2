#pragma once

#include "Core/Renderer/Region/IWorkDispatcher.h"
#include "Common/primitive_type.h"
#include "Core/Renderer/Region/Region.h"
#include "Frame/frame_fwd.h"

#include <cstddef>
#include <queue>

namespace ph
{

/*
	Regions are recursively refined and dispatched based on an error metric
	calculated from two frames. A region will not be dispatched again if its
	error is below a certain threshold. The implementation roughly follows 
	the paper written by Dammertz et al, with some modifications.

	Reference:

	"A Hierarchical Automatic Stopping Condition for Monte Carlo Global 
	Illumination", Holger Dammertz, Johannes Hanika, Alexander Keller, 
	Hendrik Lensch; Full Papers Proceedings of the WSCG 2010, p. 159-164.
*/
class DammertzAdaptiveDispatcher : public IWorkDispatcher
{
public:
	explicit DammertzAdaptiveDispatcher(const Region& fullRegion);
	DammertzAdaptiveDispatcher(
		const Region& fullRegion, 
		real precisionStandard, 
		std::size_t depthPerRegion);

	bool dispatch(WorkUnit* out_workUnit) override;

	void analyzeFinishedRegion(
		const Region& finishedRegion, 
		const HdrRgbFrame& allEffortFrame, 
		const HdrRgbFrame& halfEffortFrame);

private:
	real m_splitThreshold;
	real m_terminateThreshold;
	std::size_t m_depthPerRegion;
	std::queue<Region> m_pendingRegions;
	Region m_fullRegion;
	real m_rcpNumRegionPixels;

	void addPendingRegion(const Region& region);
};

// In-header Implementations:

inline void DammertzAdaptiveDispatcher::addPendingRegion(const Region& region)
{
	if(region.isArea())
	{
		m_pendingRegions.push(region);
	}
}

}// end namespace ph