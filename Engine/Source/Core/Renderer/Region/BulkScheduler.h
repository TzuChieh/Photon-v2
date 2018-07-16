#pragma once

#include "Core/Renderer/Region/RegionScheduler.h"
#include "Common/assertion.h"

namespace ph
{

class BulkScheduler : public RegionScheduler
{
public:
	BulkScheduler(uint32 numWorkers);

	bool getNextRegion(Region* out_region) override;

private:
	uint32 m_numWorkers;
	uint32 m_numRemainingRegions;
};

// In-header Implementations:

inline BulkScheduler::BulkScheduler(const uint32 numWorkers) :
	RegionScheduler(numWorkers),
	m_numRemainingRegions(numWorkers)
{}

inline bool BulkScheduler::getNextRegion(Region* const out_region)
{
	PH_ASSERT(out_region);

	if(m_numRemainingRegions > 0)
	{
		// TODO
	}
	else
	{
		return false;
	}
}

}// end namespace ph