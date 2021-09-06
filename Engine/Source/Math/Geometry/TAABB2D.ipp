#pragma once

#include "Math/Geometry/TAABB2D.h"
#include "Common/assertion.h"
#include "Math/math.h"

#include <string>
#include <type_traits>

namespace ph::math
{

template<typename T>
inline TAABB2D<T>::TAABB2D() :
	TAABB2D(TVector2<T>(0, 0))
{}

template<typename T>
inline TAABB2D<T>::TAABB2D(const TVector2<T>& minVertex, const TVector2<T>& maxVertex) : 
	m_minVertex(minVertex), m_maxVertex(maxVertex)
{}

template<typename T>
inline TAABB2D<T>::TAABB2D(const TVector2<T>& point) : 
	TAABB2D(point, point)
{}

template<typename T>
template<typename U>
inline TAABB2D<T>::TAABB2D(const TAABB2D<U>& other) : 
	m_minVertex(other.getMinVertex()), m_maxVertex(other.getMaxVertex())
{}

template<typename T>
inline bool TAABB2D<T>::isIntersectingArea(const TAABB2D& other) const
{
	return m_maxVertex.x() >= other.m_minVertex.x() && m_minVertex.x() <= other.m_maxVertex.x() &&
	       m_maxVertex.y() >= other.m_minVertex.y() && m_minVertex.y() <= other.m_maxVertex.y();
}

template<typename T>
inline bool TAABB2D<T>::isIntersectingArea(const TVector2<T>& point) const
{
	return point.x() >= m_minVertex.x() && point.x() <= m_maxVertex.x() &&
	       point.y() >= m_minVertex.y() && point.y() <= m_maxVertex.y();
}

template<typename T>
inline bool TAABB2D<T>::isIntersectingRange(const TVector2<T>& point) const
{
	return point.x() >= m_minVertex.x() && point.x() < m_maxVertex.x() &&
	       point.y() >= m_minVertex.y() && point.y() < m_maxVertex.y();
}

template<typename T>
inline T TAABB2D<T>::getArea() const
{
	PH_ASSERT_MSG(isValid(), toString());

	return getWidth() * getHeight();
}

template<typename T>
inline TAABB2D<T>& TAABB2D<T>::unionWith(const TAABB2D& other)
{
	m_minVertex = m_minVertex.min(other.m_minVertex);
	m_maxVertex = m_maxVertex.max(other.m_maxVertex);

	return *this;
}

template<typename T>
inline TAABB2D<T>& TAABB2D<T>::intersectWith(const TAABB2D& other)
{
	m_minVertex = m_minVertex.max(other.m_minVertex);
	m_maxVertex = m_maxVertex.min(other.m_maxVertex);

	return *this;
}

template<typename T>
inline TAABB2D<T>& TAABB2D<T>::setMinVertex(const TVector2<T>& minVertex)
{
	m_minVertex = minVertex;

	return *this;
}

template<typename T>
inline TAABB2D<T>& TAABB2D<T>::setMaxVertex(const TVector2<T>& maxVertex)
{
	m_maxVertex = maxVertex;

	return *this;
}

template<typename T>
inline TAABB2D<T>& TAABB2D<T>::setVertices(std::pair<TVector2<T>, TVector2<T>> minMaxVertices)
{
	setMinVertex(minMaxVertices.first);
	setMaxVertex(minMaxVertices.second);

	return *this;
}

template<typename T>
inline const TVector2<T>& TAABB2D<T>::getMinVertex() const
{
	return m_minVertex;
}

template<typename T>
inline const TVector2<T>& TAABB2D<T>::getMaxVertex() const
{
	return m_maxVertex;
}

template<typename T>
inline std::pair<TVector2<T>, TVector2<T>> TAABB2D<T>::getVertices() const
{
	return {getMinVertex(), getMaxVertex()};
}

template<typename T>
inline T TAABB2D<T>::getWidth() const
{
	return m_maxVertex.x() - m_minVertex.x();
}

template<typename T>
inline T TAABB2D<T>::getHeight() const
{
	return m_maxVertex.y() - m_minVertex.y();
}

template<typename T>
inline TVector2<T> TAABB2D<T>::getExtents() const
{
	return TVector2<T>(getWidth(), getHeight());
}

template<typename T>
inline TVector2<T> TAABB2D<T>::getCenter() const
{
	if constexpr(std::is_integral_v<T>)
	{
		return m_minVertex.add(m_maxVertex).div(T(2));
	}
	else
	{
		return m_minVertex.add(m_maxVertex).mul(T(0.5));
	}
}

template<typename T>
inline std::pair<TAABB2D<T>, TAABB2D<T>> TAABB2D<T>::getSplitted(const std::size_t axis, const T splitPoint) const
{
	PH_ASSERT_MSG(axis == 0 || axis == 1, std::to_string(axis));
	PH_ASSERT_IN_RANGE_INCLUSIVE(splitPoint, m_minVertex[axis], m_maxVertex[axis]);

	return {
		TAABB2D(m_minVertex, TVector2<T>(m_maxVertex).set(axis, splitPoint)),
		TAABB2D(TVector2<T>(m_minVertex).set(axis, splitPoint), m_maxVertex)
	};
}

template<typename T>
inline TAABB2D<T> TAABB2D<T>::getUnioned(const TAABB2D& other) const
{
	return TAABB2D(*this).unionWith(other);
}

template<typename T>
inline TAABB2D<T> TAABB2D<T>::getIntersected(const TAABB2D& other) const
{
	return TAABB2D(*this).intersectWith(other);
}

template<typename T>
inline bool TAABB2D<T>::isValid() const
{
	return m_minVertex.x() <= m_maxVertex.x() && m_minVertex.y() <= m_maxVertex.y();
}

template<typename T>
inline bool TAABB2D<T>::isPoint() const
{
	return m_minVertex.isEqual(m_maxVertex);
}

template<typename T>
inline bool TAABB2D<T>::isArea() const
{
	return m_maxVertex.x() > m_minVertex.x() && m_maxVertex.y() > m_minVertex.y();
}

template<typename T>
inline TVector2<T> TAABB2D<T>::sampleToSurface(const std::array<T, 2>& sample) const
{
	PH_ASSERT_IN_RANGE_INCLUSIVE(sample[0], static_cast<T>(0), static_cast<T>(1));
	PH_ASSERT_IN_RANGE_INCLUSIVE(sample[1], static_cast<T>(0), static_cast<T>(1));

	return xy01ToSurface({sample[0], sample[1]});
}

template<typename T>
inline TVector2<T> TAABB2D<T>::xy01ToSurface(const TVector2<T>& xy01) const
{
	PH_ASSERT(isValid());

	return xy01.mul(getExtents()).add(m_minVertex);
}

template<typename T>
inline bool TAABB2D<T>::isEqual(const TAABB2D& other) const
{
	return m_minVertex.isEqual(other.m_minVertex) &&
	       m_maxVertex.isEqual(other.m_maxVertex);
}

template<typename T>
inline std::string TAABB2D<T>::toString() const
{
	return "AABB: min" + m_minVertex.toString() + ", max" + m_maxVertex.toString();
}

}// end namespace ph::math
