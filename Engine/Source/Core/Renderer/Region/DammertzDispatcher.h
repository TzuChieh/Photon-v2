#pragma once

#include "Core/Renderer/Region/IWorkDispatcher.h"
#include "Common/primitive_type.h"
#include "Core/Renderer/Region/Region.h"
#include "Frame/frame_fwd.h"

#include <cstddef>
#include <queue>
#include <utility>
#include <vector>

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
class DammertzDispatcher : public IWorkDispatcher
{
public:
	class Analyzer;

	DammertzDispatcher() = default;

	explicit DammertzDispatcher(
		const Region& fullRegion);

	DammertzDispatcher(
		const Region& fullRegion, 
		real          precisionStandard, 
		std::size_t   depthPerRegion);

	bool dispatch(WorkUnit* out_workUnit) override;

	Analyzer createAnalyzer() const;
	void addAnalyzedData(const Analyzer& analyzer);

	class Analyzer final
	{
		friend DammertzDispatcher;

	public:
		void analyzeFinishedRegion(
			const Region&      finishedRegion,
			const HdrRgbFrame& allEffortFrame,
			const HdrRgbFrame& halfEffortFrame);

	private:
		Analyzer(
			real splitThreshold,
			real terminateThreshold,
			real numFullRegionPixels);

		std::pair<Region, Region> getNextRegions() const;

		real                      m_splitThreshold;
		real                      m_terminateThreshold;
		std::pair<Region, Region> m_nextRegions;
		real                      m_rcpNumRegionPixels;
		std::vector<real>         m_accumulatedErrors;
	};

private:
	constexpr static std::size_t MIN_REGION_AREA = 16 * 16;

	real               m_splitThreshold;
	real               m_terminateThreshold;
	std::size_t        m_depthPerRegion;
	std::queue<Region> m_pendingRegions;
	Region             m_fullRegion;

	void addPendingRegion(const Region& region);
};

// In-header Implementations:

inline DammertzDispatcher::Analyzer DammertzDispatcher::createAnalyzer() const
{
	return Analyzer(
		m_splitThreshold, 
		m_terminateThreshold, 
		static_cast<real>(m_fullRegion.calcArea()));
}

inline void DammertzDispatcher::addAnalyzedData(const Analyzer& analyzer)
{
	const auto nextRegions = analyzer.getNextRegions();
	addPendingRegion(nextRegions.first);
	addPendingRegion(nextRegions.second);
}

inline DammertzDispatcher::Analyzer::Analyzer(
	const real splitThreshold,
	const real terminateThreshold,
	const real numFullRegionPixels) : 

	m_splitThreshold    (splitThreshold),
	m_terminateThreshold(terminateThreshold),
	m_nextRegions       (Region({0, 0}), Region({0, 0})),
	m_rcpNumRegionPixels(1.0_r / numFullRegionPixels),
	m_accumulatedErrors (MIN_REGION_AREA)
{}

inline void DammertzDispatcher::addPendingRegion(const Region& region)
{
	if(region.isArea())
	{
		m_pendingRegions.push(region);
	}
}

inline std::pair<Region, Region> DammertzDispatcher::Analyzer::getNextRegions() const
{
	return m_nextRegions;
}

}// end namespace ph