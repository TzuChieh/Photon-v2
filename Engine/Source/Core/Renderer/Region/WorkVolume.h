#pragma once

#include "Core/Bound/TAABB2D.h"
#include "Common/assertion.h"

namespace ph
{

// TODO: inherit from TAABB3D
class WorkVolume
{
public:
	WorkVolume();
	WorkVolume(const TAABB2D<std::size_t>& area);
	WorkVolume(const TAABB2D<std::size_t>& area, std::size_t depth);

	TAABB2D<std::size_t> getWorkArea() const
	{
		return m_area;
	}

	std::size_t getWorkDepth() const
	{
		return m_depth;
	}

private:
	TAABB2D<std::size_t> m_area;
	std::size_t          m_depth;
};

// In-header Implementations:

inline WorkVolume::WorkVolume() : 
	WorkVolume({{0, 0}, {0, 0}}, 0)
{}

inline WorkVolume::WorkVolume(const TAABB2D<std::size_t>& area) :
	WorkVolume(area, 1)
{}

inline WorkVolume::WorkVolume(const TAABB2D<std::size_t>& area, const std::size_t depth) :
	m_area(area),
	m_depth(depth)
{
	PH_ASSERT_MSG(m_area.isValid(), "area = " + m_area.toString());
}

}// end namespace ph