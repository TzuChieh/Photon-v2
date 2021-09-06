#pragma once

#include "Math/TVector2.h"
#include "Math/Geometry/TAABB2D.h"
#include "Common/assertion.h"

#include <utility>
#include <cstddef>
#include <string>

namespace ph
{

class Viewport final
{
public:
	Viewport();
	explicit Viewport(math::TVector2<uint32> baseSizePx);
	Viewport(math::TVector2<uint32> baseSizePx, math::TAABB2D<int64> windowPx);

	const math::TVector2<uint32>& getBaseSizePx() const;
	const math::TAABB2D<int64>& getWindowPx() const;
	math::TAABB2D<int64> getCroppedRegionPx() const;
	std::size_t numBasePixels() const;
	std::size_t numWindowPixels() const;
	bool hasView() const;
	std::string toString() const;

private:
	math::TVector2<uint32> m_baseSizePx;
	math::TAABB2D<int64>   m_windowPx;
};

// In-header Implementations:

inline Viewport::Viewport() :
	Viewport({0, 0})
{}

inline Viewport::Viewport(math::TVector2<uint32> baseSizePx) :
	Viewport(std::move(baseSizePx), math::TAABB2D<int64>({0, 0}, {baseSizePx.x(), baseSizePx.y() }))
{}

inline Viewport::Viewport(math::TVector2<uint32> baseSizePx, math::TAABB2D<int64> windowPx) :
	m_baseSizePx(std::move(baseSizePx)),
	m_windowPx  (std::move(windowPx))
{
	PH_ASSERT(m_windowPx.isValid());
}

inline const math::TVector2<uint32>& Viewport::getBaseSizePx() const
{
	return m_baseSizePx;
}

inline const math::TAABB2D<int64>& Viewport::getWindowPx() const
{
	return m_windowPx;
}

inline math::TAABB2D<int64> Viewport::getCroppedRegionPx() const
{
	math::TAABB2D<int64> intersectedWindowPx({0, 0}, {m_baseSizePx.x(), m_baseSizePx.y() });
	intersectedWindowPx.intersectWith(m_windowPx);
	return intersectedWindowPx;
}

inline std::size_t Viewport::numBasePixels() const
{
	return static_cast<std::size_t>(m_baseSizePx.x()) * m_baseSizePx.y();
}

inline std::size_t Viewport::numWindowPixels() const
{
	return static_cast<std::size_t>(m_windowPx.getArea());
}

inline bool Viewport::hasView() const
{
	return getCroppedRegionPx().getArea() > 0;
}

inline std::string Viewport::toString() const
{
	return "viewport base: " + m_baseSizePx.toString() + ", window: " + m_windowPx.toString();
}

}// end namespace ph
