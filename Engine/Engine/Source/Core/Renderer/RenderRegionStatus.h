#pragma once

#include "Core/Scheduler/Region.h"
#include "Core/Renderer/ERegionStatus.h"

#include <algorithm>

namespace ph
{

class RenderRegionStatus final
{
public:
	RenderRegionStatus();
	RenderRegionStatus(Region region, ERegionStatus status);

	Region getRegion() const;
	ERegionStatus getStatus() const;
	RenderRegionStatus getMerged(const RenderRegionStatus& other) const;

private:
	Region m_region;
	ERegionStatus m_status;
};

inline RenderRegionStatus::RenderRegionStatus()
	: RenderRegionStatus(Region::makeEmpty(), ERegionStatus::Invalid)
{}

inline RenderRegionStatus::RenderRegionStatus(Region region, ERegionStatus status)
	: m_region(region)
	, m_status(status)
{}

inline Region RenderRegionStatus::getRegion() const
{
	return m_region;
}

inline ERegionStatus RenderRegionStatus::getStatus() const
{
	return m_status;
}

inline RenderRegionStatus RenderRegionStatus::getMerged(const RenderRegionStatus& other) const
{
	// Larger status value take precedence, see `ERegionStatus`
	return RenderRegionStatus(
		m_region.getUnioned(other.m_region),
		std::max(m_status, other.m_status));
}

}// end namespace ph
