#pragma once

#include "Core/Renderer/Region/Region.h"
#include "Common/primitive_type.h"

namespace ph
{

class RegionScheduler
{
public:
	// TODO: input fullRegion
	RegionScheduler(uint32 numWorkers);
	virtual ~RegionScheduler() = default;

	virtual bool getNextRegion(Region* out_region) = 0;

	inline uint32 getNumWorkers() const;

private:
	uint32 m_numWorkers;
};

// In-header Implementations:

RegionScheduler::RegionScheduler(const uint32 numWorkers) : 
	m_numWorkers(numWorkers)
{}

inline uint32 RegionScheduler::getNumWorkers() const
{
	return m_numWorkers;
}

}// end namespace ph