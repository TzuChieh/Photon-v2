#pragma once

#include "Core/Renderer/Region/IWorkDispatcher.h"
#include "Common/primitive_type.h"
#include "Core/Renderer/Region/Region.h"
#include "Common/assertion.h"
#include "Frame/TFrame.h"
#include "Math/math.h"

#include <cmath>
#include <string>
#include <iostream>
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
	enum class ERefineMode
	{
		MIDPOINT,
		MIN_ERROR_DIFFERENCE
	};

	template<ERefineMode MODE>
	class TAnalyzer;

	DammertzDispatcher() = default;

	explicit DammertzDispatcher(
		const Region& fullRegion);

	DammertzDispatcher(
		const Region& fullRegion, 
		real          precisionStandard, 
		std::size_t   depthPerRegion);

	bool dispatch(WorkUnit* out_workUnit) override;

	template<ERefineMode MODE>
	TAnalyzer<MODE> createAnalyzer() const;

	template<ERefineMode MODE>
	void addAnalyzedData(const TAnalyzer<MODE>& analyzer);

	template<ERefineMode MODE>
	class TAnalyzer final
	{
		friend DammertzDispatcher;

	public:
		void analyzeFinishedRegion(
			const Region&      finishedRegion,
			const HdrRgbFrame& allEffortFrame,
			const HdrRgbFrame& halfEffortFrame);

	private:
		TAnalyzer(
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

template<DammertzDispatcher::ERefineMode MODE>
inline DammertzDispatcher::TAnalyzer<MODE> DammertzDispatcher::createAnalyzer() const
{
	return TAnalyzer<MODE>(
		m_splitThreshold, 
		m_terminateThreshold, 
		static_cast<real>(m_fullRegion.calcArea()));
}

template<DammertzDispatcher::ERefineMode MODE>
inline void DammertzDispatcher::addAnalyzedData(const TAnalyzer<MODE>& analyzer)
{
	const auto nextRegions = analyzer.getNextRegions();
	addPendingRegion(nextRegions.first);
	addPendingRegion(nextRegions.second);
}

template<DammertzDispatcher::ERefineMode MODE>
inline void DammertzDispatcher::TAnalyzer<MODE>::analyzeFinishedRegion(
	const Region&      finishedRegion,
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
		m_nextRegions.first  = finishedRegion;
		m_nextRegions.second = Region({0, 0});

		std::cerr << "too large, split = " << m_splitThreshold << std::endl;
	}
	else if(regionError >= m_terminateThreshold)
	{
		// TODO: split on the point that minimizes the difference of error across two splitted regions

		if(finishedRegion.calcArea() >= MIN_REGION_AREA)
		{
			// error is small, splitted and added for more effort
			const int maxDimension = finishedRegion.getExtents().maxDimension();
			const int64 midPoint = (finishedRegion.minVertex[maxDimension] + finishedRegion.maxVertex[maxDimension]) / 2;

			m_nextRegions = finishedRegion.getSplitted(maxDimension, midPoint);

			std::cerr << "small, splitted, terminate = " << m_terminateThreshold << std::endl;
		}
		else
		{
			m_nextRegions.first  = finishedRegion;
			m_nextRegions.second = Region({0, 0});

			std::cerr << "small, region too small, not splitted, terminate = " << m_terminateThreshold << std::endl;
		}
	}
	else
	{
		// error is very small, no further effort needed
		m_nextRegions.first  = Region({0, 0});
		m_nextRegions.second = Region({0, 0});

		std::cerr << "very small, terminated" << std::endl;
	}
}

template<DammertzDispatcher::ERefineMode MODE>
inline DammertzDispatcher::TAnalyzer<MODE>::TAnalyzer(
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

template<DammertzDispatcher::ERefineMode MODE>
inline std::pair<Region, Region> DammertzDispatcher::TAnalyzer<MODE>::getNextRegions() const
{
	return m_nextRegions;
}

}// end namespace ph