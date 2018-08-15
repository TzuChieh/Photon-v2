#pragma once

#include "Core/Renderer/Region/RegionScheduler.h"
#include "Common/assertion.h"

#include <algorithm>

namespace ph
{

class BulkScheduler : public RegionScheduler
{
public:
	BulkScheduler() = default;

	void init() override;
	bool scheduleRegion(Region* out_region, uint64* out_spp) override;
	void percentageProgress(float* out_worst, float* out_best) const override;

private:
	uint64 m_remainingSpp;
};

// In-header Implementations:

inline void BulkScheduler::init()
{
	m_remainingSpp = m_sppBudget;
}

inline bool BulkScheduler::scheduleRegion(Region* const out_region, uint64* const out_spp)
{
	PH_ASSERT(out_region && out_spp && m_numWorkers > 0);

	if(m_remainingSpp == 0)
	{
		return false;
	}

	const uint64 sppPerWorker = std::max(m_sppBudget / m_numWorkers, uint64(1));
	if(m_remainingSpp >= sppPerWorker)
	{
		*out_region = m_fullRegion;
		*out_spp    = sppPerWorker;
		m_remainingSpp -= sppPerWorker;
		return true;
	}
	else
	{
		*out_region = m_fullRegion;
		*out_spp    = m_remainingSpp;
		m_remainingSpp = 0;
		return true;
	}
}

inline void BulkScheduler::percentageProgress(
	float* const out_worst, 
	float* const out_best) const
{
	PH_ASSERT(out_worst && out_best);

	const float remaining = static_cast<float>(m_remainingSpp) / static_cast<float>(m_sppBudget);

	// Worst is 0% since we do not know whether the dispatched regions are 
	// done or not.
	*out_worst = 0.0f;
	*out_best  = (1.0f - remaining) * 100.0f;
}

}// end namespace ph