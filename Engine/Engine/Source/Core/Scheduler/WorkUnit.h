#pragma once

#include "Math/Geometry/TAABB2D.h"
#include "Common/assertion.h"
#include "Core/Scheduler/Region.h"

namespace ph
{

/*! @brief Represents some amount of work.

Represents some amount of work as a volume with specific dimensions. The 
actual units of the dimensions are depend on the use case.
*/
class WorkUnit
{
public:
	/*! @brief Represents zero amount of work.
	*/
	WorkUnit();

	/*! @brief Represents a rectangular region of work with unit depth.
	*/
	explicit WorkUnit(const Region& region);

	/*! @brief Represents a volume of work with specified dimensions.
	*/
	WorkUnit(const Region& region, std::size_t depth);

	Region getRegion() const;
	int64 getWidth() const;
	int64 getHeight() const;
	std::size_t getDepth() const;
	std::size_t getVolume() const;
	float64 getAspectRatio() const;

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
	PH_ASSERT_MSG(!m_region.isEmpty(), "region = " + m_region.toString());
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
	PH_ASSERT(!m_region.isEmpty());

	return static_cast<std::size_t>(m_region.getArea()) * m_depth;
}

inline float64 WorkUnit::getAspectRatio() const
{
	PH_ASSERT_GE(m_region.getWidth(),  0);
	PH_ASSERT_GT(m_region.getHeight(), 0);

	return static_cast<float64>(m_region.getWidth()) / static_cast<float64>(m_region.getHeight());
}

inline std::string WorkUnit::toString() const
{
	return "region = " + m_region.toString() + ", depth = " + std::to_string(m_depth);
}

}// end namespace ph
