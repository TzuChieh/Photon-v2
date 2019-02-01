#pragma once

#include "Core/Bound/TAABB2D.h"
#include "Common/assertion.h"
#include "Core/Renderer/Region/Region.h"

#include <iostream>

namespace ph
{

class WorkUnit
{
public:
	// Represents zero amount of work.
	WorkUnit();

	WorkUnit(const Region& region);
	WorkUnit(const Region& region, std::size_t depth);

	Region getRegion() const;
	int64 getWidth() const;
	int64 getHeight() const;
	std::size_t getDepth() const;
	std::size_t getVolume() const;

	std::string toString() const;

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

inline int64 WorkUnit::getWidth() const
{
	return m_region.getWidth();
}

inline int64 WorkUnit::getHeight() const
{
	return m_region.getHeight();
}

inline Region WorkUnit::getRegion() const
{
	return m_region;
}

inline std::size_t WorkUnit::getDepth() const
{
	return m_depth;
}

inline std::size_t WorkUnit::getVolume() const
{
	PH_ASSERT(m_region.isValid());

	return static_cast<std::size_t>(m_region.calcArea()) * m_depth;
}

inline std::string WorkUnit::toString() const
{
	return "region = " + m_region.toString() + ", depth = " + std::to_string(m_depth);
}

}// end namespace ph