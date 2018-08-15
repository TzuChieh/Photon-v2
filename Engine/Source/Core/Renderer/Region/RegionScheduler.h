#pragma once

#include "Core/Renderer/Region/Region.h"
#include "Common/primitive_type.h"

namespace ph
{

class RegionScheduler
{
public:
	RegionScheduler() = default;
	virtual ~RegionScheduler() = default;

	virtual void init() = 0;
	virtual bool scheduleRegion(Region* out_region, uint64* out_spp) = 0;
	virtual void percentageProgress(float* out_worst, float* out_best) const = 0;

	uint32 getNumWorkers() const;
	Region getFullRegion() const;
	uint64 getSppBudget()  const;
	void setNumWorkers(uint32 numWorkers);
	void setFullRegion(const Region& fullRegion);
	void setSppBudget(uint64 sppBudget);

protected:
	uint32 m_numWorkers;
	Region m_fullRegion;
	uint64 m_sppBudget;
};

// In-header Implementations:

inline uint32 RegionScheduler::getNumWorkers() const
{
	return m_numWorkers;
}

inline Region RegionScheduler::getFullRegion() const
{
	return m_fullRegion;
}

inline void RegionScheduler::setNumWorkers(const uint32 numWorkers)
{
	m_numWorkers = numWorkers;
}

inline void RegionScheduler::setFullRegion(const Region& fullRegion)
{
	m_fullRegion = fullRegion;
}

inline uint64 RegionScheduler::getSppBudget() const
{
	return m_sppBudget;
}

inline void RegionScheduler::setSppBudget(const uint64 sppBudget)
{
	m_sppBudget = sppBudget;
}

}// end namespace ph