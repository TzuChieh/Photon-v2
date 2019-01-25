#pragma once

#include "Core/Bound/TAABB2D.h"
#include "Common/assertion.h"
#include "Core/Renderer/Region/Region.h"

namespace ph
{

// TODO: inherit from TAABB3D
class WorkUnit
{
public:
	WorkUnit();
	WorkUnit(const Region& region);
	WorkUnit(const Region& region, std::size_t depth);

	Region getRegion() const
	{
		return m_region;
	}

	std::size_t getDepth() const
	{
		return m_depth;
	}

	std::size_t getVolume() const
	{
		PH_ASSERT(m_region.isValid());

		return static_cast<std::size_t>(m_region.calcArea()) * m_depth;
	}

private:
	Region      m_region;
	std::size_t m_depth;
};

// In-header Implementations:

inline WorkUnit::WorkUnit() :
	WorkUnit({{0, 0}, {0, 0}}, 0)
{}

inline WorkUnit::WorkUnit(const Region& region) :
	WorkUnit(region, 1)
{}

inline WorkUnit::WorkUnit(const Region& region, const std::size_t depth) :
	m_region(region),
	m_depth(depth)
{
	PH_ASSERT_MSG(m_region.isValid(), "region = " + m_region.toString());
}

}// end namespace ph