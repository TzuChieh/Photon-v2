#pragma once

#include "Core/Scheduler/IWorkDispatcher.h"
#include "Common/primitive_type.h"
#include "Core/Scheduler/Region.h"
#include "Common/assertion.h"
#include "Frame/TFrame.h"
#include "Math/math.h"

#include <cmath>
#include <cstddef>
#include <queue>
#include <utility>
#include <vector>
#include <limits>
#include <algorithm>

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
		uint32        numWorkers,
		const Region& fullRegion);

	DammertzDispatcher(
		uint32        numWorkers,
		const Region& fullRegion, 
		real          precisionStandard, 
		std::size_t   initialDepthPerRegion);

	DammertzDispatcher(
		uint32        numWorkers,
		const Region& fullRegion,
		real          precisionStandard,
		std::size_t   initialDepthPerRegion,
		std::size_t   standardDepthPerRegion,
		std::size_t   terminusDepthPerRegion);

	bool dispatch(WorkUnit* out_workUnit) override;

	template<ERefineMode MODE>
	TAnalyzer<MODE> createAnalyzer() const;

	template<ERefineMode MODE>
	void addAnalyzedData(const TAnalyzer<MODE>& analyzer);

	std::size_t numPendingRegions() const;

	template<ERefineMode MODE>
	class TAnalyzer final
	{
		friend DammertzDispatcher;

	public:
		void analyzeFinishedRegion(
			const Region&      finishedRegion,
			const HdrRgbFrame& allEffortFrame,
			const HdrRgbFrame& halfEffortFrame);

		bool isConverged() const;

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
		std::vector<real>         m_accumulatedEps;
	};

private:
	constexpr static std::size_t MIN_REGION_AREA = 256;

	real                 m_splitThreshold;
	real                 m_terminateThreshold;
	std::size_t          m_standardDepthPerRegion;
	std::size_t          m_terminusDepthPerRegion;
	Region               m_fullRegion;
	std::queue<WorkUnit> m_pendingRegions;

	void addAnalyzedRegion(const Region& region);
};

// In-header Implementations:

template<DammertzDispatcher::ERefineMode MODE>
inline DammertzDispatcher::TAnalyzer<MODE> DammertzDispatcher::createAnalyzer() const
{
	return TAnalyzer<MODE>(
		m_splitThreshold, 
		m_terminateThreshold, 
		static_cast<real>(m_fullRegion.getArea()));
}

template<DammertzDispatcher::ERefineMode MODE>
inline void DammertzDispatcher::addAnalyzedData(const TAnalyzer<MODE>& analyzer)
{
	const auto nextRegions = analyzer.getNextRegions();
	addAnalyzedRegion(nextRegions.first);
	addAnalyzedRegion(nextRegions.second);
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
	m_accumulatedEps    ()
{}

inline std::size_t DammertzDispatcher::numPendingRegions() const
{
	return m_pendingRegions.size();
}

template<DammertzDispatcher::ERefineMode MODE>
inline std::pair<Region, Region> DammertzDispatcher::TAnalyzer<MODE>::getNextRegions() const
{
	return m_nextRegions;
}

template<DammertzDispatcher::ERefineMode MODE>
inline bool DammertzDispatcher::TAnalyzer<MODE>::isConverged() const
{
	return !m_nextRegions.first.isArea() && !m_nextRegions.second.isArea();
}

template<>
inline void DammertzDispatcher::TAnalyzer<DammertzDispatcher::ERefineMode::MIDPOINT>::analyzeFinishedRegion(
	const Region&      finishedRegion,
	const HdrRgbFrame& allEffortFrame,
	const HdrRgbFrame& halfEffortFrame)
{
	using namespace math;

	PH_ASSERT_GE(finishedRegion.getMinVertex().x, 0);
	PH_ASSERT_GE(finishedRegion.getMinVertex().y, 0);
	PH_ASSERT_LE(finishedRegion.getWidth(),  allEffortFrame.widthPx());
	PH_ASSERT_LE(finishedRegion.getHeight(), allEffortFrame.heightPx());
	PH_ASSERT_LE(finishedRegion.getWidth(),  halfEffortFrame.widthPx());
	PH_ASSERT_LE(finishedRegion.getHeight(), halfEffortFrame.heightPx());
	const TAABB2D<uint32> frameRegion(finishedRegion);

	real regionError = 0;
	for(uint32 y = frameRegion.getMinVertex().y; y < frameRegion.getMaxVertex().y; ++y)
	{
		for(uint32 x = frameRegion.getMinVertex().x; x < frameRegion.getMaxVertex().x; ++x)
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
	regionError /= frameRegion.getArea();
	regionError *= fast_sqrt(frameRegion.getArea() * m_rcpNumRegionPixels);
	PH_ASSERT_MSG(std::isfinite(regionError), std::to_string(regionError));

	if(regionError >= m_splitThreshold)
	{
		// error is large, added for more effort
		m_nextRegions.first  = finishedRegion;
		m_nextRegions.second = Region({0, 0});
	}
	else if(regionError >= m_terminateThreshold)
	{
		if(finishedRegion.getArea() >= MIN_REGION_AREA)
		{
			// error is small, splitted and added for more effort
			const auto  maxDimension = finishedRegion.getExtents().maxDimension();
			const int64 midPoint     = (finishedRegion.getMinVertex()[maxDimension] + finishedRegion.getMaxVertex()[maxDimension]) / 2;

			m_nextRegions = finishedRegion.getSplitted(maxDimension, midPoint);
		}
		else
		{
			m_nextRegions.first  = finishedRegion;
			m_nextRegions.second = Region({0, 0});
		}
	}
	else
	{
		// error is very small, no further effort needed
		m_nextRegions.first  = Region({0, 0});
		m_nextRegions.second = Region({0, 0});
	}
}

template<>
inline void DammertzDispatcher::TAnalyzer<DammertzDispatcher::ERefineMode::MIN_ERROR_DIFFERENCE>::analyzeFinishedRegion(
	const Region&      finishedRegion,
	const HdrRgbFrame& allEffortFrame,
	const HdrRgbFrame& halfEffortFrame)
{
	using namespace math;

	PH_ASSERT_GE(finishedRegion.getMinVertex().x, 0);
	PH_ASSERT_GE(finishedRegion.getMinVertex().y, 0);
	PH_ASSERT_LE(finishedRegion.getWidth(),  allEffortFrame.widthPx());
	PH_ASSERT_LE(finishedRegion.getHeight(), allEffortFrame.heightPx());
	PH_ASSERT_LE(finishedRegion.getWidth(),  halfEffortFrame.widthPx());
	PH_ASSERT_LE(finishedRegion.getHeight(), halfEffortFrame.heightPx());
	const TAABB2D<uint32> frameRegion(finishedRegion);

	const auto regionExtents = frameRegion.getExtents();
	const auto maxDimension  = regionExtents.maxDimension();

	m_accumulatedEps.resize(regionExtents[maxDimension]);
	std::fill(m_accumulatedEps.begin(), m_accumulatedEps.end(), 0.0_r);

	real summedEp = 0;
	for(uint32 y = frameRegion.getMinVertex().y; y < frameRegion.getMaxVertex().y; ++y)
	{
		real summedRowEp = 0;
		for(uint32 x = frameRegion.getMinVertex().x; x < frameRegion.getMaxVertex().x; ++x)
		{
			HdrRgbFrame::Pixel I, A;
			allEffortFrame.getPixel(x, y, &I);
			halfEffortFrame.getPixel(x, y, &A);

			const real numerator      = I.sub(A).abs().sum();
			const real sumOfI         = I.sum();
			const real rcpDenominator = sumOfI > 0 ? fast_rcp_sqrt(sumOfI) : 0;

			PH_ASSERT_GE(numerator * rcpDenominator, 0);
			summedRowEp += numerator * rcpDenominator;

			if(maxDimension == constant::X_AXIS)
			{
				m_accumulatedEps[x - frameRegion.getMinVertex().x] += summedRowEp;
			}
		}
		summedEp += summedRowEp;

		if(maxDimension == constant::Y_AXIS)
		{
			m_accumulatedEps[y - frameRegion.getMinVertex().y] = summedEp;
		}
	}

	real regionError = summedEp;
	regionError /= frameRegion.getArea();
	regionError *= fast_sqrt(frameRegion.getArea() * m_rcpNumRegionPixels);
	PH_ASSERT_MSG(regionError > 0 && std::isfinite(regionError), std::to_string(regionError));

	if(regionError >= m_splitThreshold)
	{
		// error is large, added for more effort
		m_nextRegions.first  = finishedRegion;
		m_nextRegions.second = Region({0, 0});
	}
	else if(regionError >= m_terminateThreshold)
	{
		if(finishedRegion.getArea() >= MIN_REGION_AREA)
		{
			// Split on the point that minimizes the difference of error 
			// across two splitted regions. To find the point, we squared the
			// error metric (to avoid sqrt) and stripped away some constants
			// which do not affect the result.

			const real totalEps = m_accumulatedEps.back();

			int64 bestPosPx    = 0;
			real  minErrorDiff = totalEps * fast_rcp_sqrt(static_cast<real>(m_accumulatedEps.size()));
			for(std::size_t i = 0; i < m_accumulatedEps.size(); ++i)
			{
				const real summedEp0 = m_accumulatedEps[i];
				const real summedEp1 = totalEps - summedEp0;
				PH_ASSERT_GE(summedEp0, 0);
				PH_ASSERT_GE(summedEp1, 0);

				const real error0    = summedEp0 * fast_rcp_sqrt(static_cast<real>(i + 1));
				const real error1    = summedEp1 * (i != m_accumulatedEps.size() - 1 ? 
					fast_rcp_sqrt(static_cast<real>(m_accumulatedEps.size() - i - 1)) : 0);
				const real errorDiff = std::abs(error0 - error1);

				if(errorDiff < minErrorDiff)
				{
					minErrorDiff = errorDiff;
					bestPosPx    = static_cast<int64>(i + 1);
				}
			}

			m_nextRegions = finishedRegion.getSplitted(
				maxDimension, 
				finishedRegion.getMinVertex()[maxDimension] + bestPosPx);
		}
		else
		{
			m_nextRegions.first  = finishedRegion;
			m_nextRegions.second = Region({0, 0});
		}
	}
	else
	{
		// error is very small, no further effort needed
		m_nextRegions.first  = Region({0, 0});
		m_nextRegions.second = Region({0, 0});
	}
}

inline void DammertzDispatcher::addAnalyzedRegion(const Region& region)
{
	if(region.isArea())
	{
		if(region.getArea() <= MIN_REGION_AREA)
		{
			m_pendingRegions.push(WorkUnit(region, m_terminusDepthPerRegion));
		}
		else
		{
			m_pendingRegions.push(WorkUnit(region, m_standardDepthPerRegion));
		}
	}
}

}// end namespace ph
