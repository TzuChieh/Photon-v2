#pragma once

#include "Math/Geometry/TAABB3D.h"
#include "Common/assertion.h"

#include <algorithm>
#include <cmath>
#include <type_traits>

namespace ph::math
{

template<typename T>
inline TAABB3D<T> TAABB3D<T>::makeUnioned(const TAABB3D& a, const TAABB3D& b)
{
	return TAABB3D(a).unionWith(b);
}

template<typename T>
inline TAABB3D<T>::TAABB3D() :
	TAABB3D(TVector3<T>(0, 0, 0))
{}

template<typename T>
inline TAABB3D<T>::TAABB3D(const TVector3<T>& point) :
	TAABB3D(point, point)
{}

template<typename T>
inline TAABB3D<T>::TAABB3D(const TVector3<T>& minVertex, const TVector3<T>& maxVertex) :
	m_minVertex(minVertex), m_maxVertex(maxVertex)
{
	PH_ASSERT_MSG(maxVertex.x() >= minVertex.x() &&
	              maxVertex.y() >= minVertex.y() &&
	              maxVertex.z() >= minVertex.z(),
		"minVertex = " + minVertex.toString() +
		"maxVertex = " + maxVertex.toString());
}

// REFACTOR: this method is duplicated with isIntersectingVolume(3)
template<typename T>
inline bool TAABB3D<T>::isIntersectingVolume(const TLineSegment<T>& segment) const
{
	T nearHitT, farHitT;
	return isIntersectingVolume(segment, &nearHitT, &farHitT);
}

/*
	Reference: Kay and Kayjia's "slab method" from a project of the ACM 
	SIGGRAPH Education Committee named HyperGraph.
*/
template<typename T>
inline bool TAABB3D<T>::isIntersectingVolume(
	const TLineSegment<T>& segment,
	T* const               out_nearHitT,
	T* const               out_farHitT) const
{
	PH_ASSERT(out_nearHitT);
	PH_ASSERT(out_farHitT);

	// The starting ray interval (tMin, tMax) will be incrementally intersect
	// against each ray-slab hitting interval (t1, t2) and be updated with the
	// resulting interval.
	//
	// Note that the following implementation is NaN-aware 
	// (tMin & tMax will never have NaNs)

	PH_ASSERT(!std::isnan(segment.getMinT()) && !std::isnan(segment.getMaxT()));

	T tMin = segment.getMinT();
	T tMax = segment.getMaxT();

	// find ray-slab hitting interval in x-axis then intersect with (tMin, tMax)

	T rcpDir = T(1) / segment.getDirection().x();
	T t1     = (m_minVertex.x() - segment.getOrigin().x()) * rcpDir;
	T t2     = (m_maxVertex.x() - segment.getOrigin().x()) * rcpDir;

	if(t1 < t2)
	{
		tMin = t1 > tMin ? t1 : tMin;
		tMax = t2 < tMax ? t2 : tMax;
	}
	else
	{
		tMin = t2 > tMin ? t2 : tMin;
		tMax = t1 < tMax ? t1 : tMax;
	}

	if(tMin > tMax)
	{
		return false;
	}

	// find ray-slab hitting interval in y-axis then intersect with (tMin, tMax)

	rcpDir = T(1) / segment.getDirection().y();
	t1     = (m_minVertex.y() - segment.getOrigin().y()) * rcpDir;
	t2     = (m_maxVertex.y() - segment.getOrigin().y()) * rcpDir;

	if(t1 < t2)
	{
		tMin = t1 > tMin ? t1 : tMin;
		tMax = t2 < tMax ? t2 : tMax;
	}
	else
	{
		tMin = t2 > tMin ? t2 : tMin;
		tMax = t1 < tMax ? t1 : tMax;
	}

	if(tMin > tMax)
	{
		return false;
	}

	// find ray-slab hitting interval in z-axis then intersect with (tMin, tMax)

	rcpDir = T(1) / segment.getDirection().z();
	t1     = (m_minVertex.z() - segment.getOrigin().z()) * rcpDir;
	t2     = (m_maxVertex.z() - segment.getOrigin().z()) * rcpDir;

	if(t1 < t2)
	{
		tMin = t1 > tMin ? t1 : tMin;
		tMax = t2 < tMax ? t2 : tMax;
	}
	else
	{
		tMin = t2 > tMin ? t2 : tMin;
		tMax = t1 < tMax ? t1 : tMax;
	}

	if(tMin > tMax)
	{
		return false;
	}

	*out_nearHitT = tMin;
	*out_farHitT  = tMax;
	return true;
}

template<typename T>
inline bool TAABB3D<T>::isIntersectingVolume(const TAABB3D& other) const
{
	return m_minVertex.x() <= other.m_maxVertex.x() && m_maxVertex.x() >= other.m_minVertex.x() &&
	       m_minVertex.y() <= other.m_maxVertex.y() && m_maxVertex.y() >= other.m_minVertex.y() &&
	       m_minVertex.z() <= other.m_maxVertex.z() && m_maxVertex.z() >= other.m_minVertex.z();
}

template<typename T>
inline TAABB3D<T>& TAABB3D<T>::unionWith(const TAABB3D& other)
{
	m_minVertex = m_minVertex.min(other.getMinVertex());
	m_maxVertex = m_maxVertex.max(other.getMaxVertex());

	return *this;
}

template<typename T>
inline TAABB3D<T>& TAABB3D<T>::unionWith(const TVector3<T>& point)
{
	m_minVertex = m_minVertex.min(point);
	m_maxVertex = m_maxVertex.max(point);

	return *this;
}

template<typename T>
inline std::pair<TVector3<T>, TVector3<T>> TAABB3D<T>::getVertices() const
{
	return {getMinVertex(), getMaxVertex()};
}

template<typename T>
inline std::array<TVector3<T>, 8> TAABB3D<T>::getBoundVertices() const
{
	return {TVector3<T>(m_minVertex.x(), m_minVertex.y(), m_minVertex.z()),
	        TVector3<T>(m_maxVertex.x(), m_minVertex.y(), m_minVertex.z()),
	        TVector3<T>(m_minVertex.x(), m_maxVertex.y(), m_minVertex.z()),
	        TVector3<T>(m_minVertex.x(), m_minVertex.y(), m_maxVertex.z()),
	        TVector3<T>(m_maxVertex.x(), m_maxVertex.y(), m_minVertex.z()),
	        TVector3<T>(m_minVertex.x(), m_maxVertex.y(), m_maxVertex.z()),
	        TVector3<T>(m_maxVertex.x(), m_minVertex.y(), m_maxVertex.z()),
	        TVector3<T>(m_maxVertex.x(), m_maxVertex.y(), m_maxVertex.z())};
}

template<typename T>
inline const TVector3<T>& TAABB3D<T>::getMinVertex() const
{
	return m_minVertex;
}

template<typename T>
inline const TVector3<T>& TAABB3D<T>::getMaxVertex() const
{
	return m_maxVertex;
}

template<typename T>
inline TVector3<T> TAABB3D<T>::getCentroid() const
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
inline TVector3<T> TAABB3D<T>::getExtents() const
{
	return m_maxVertex.sub(m_minVertex);
}

template<typename T>
inline T TAABB3D<T>::getSurfaceArea() const
{
	const TVector3<T>& extents = getExtents();
	return T(2) * (extents.x() * extents.y() + extents.y() * extents.z() + extents.z() * extents.x());
}

template<typename T>
inline T TAABB3D<T>::getVolume() const
{
	const TVector3<T>& extents = getExtents();
	return extents.x() * extents.y() * extents.z();
}

template<typename T>
inline TAABB3D<T>& TAABB3D<T>::setMinVertex(const TVector3<T>& minVertex)
{
	m_minVertex = minVertex;

	return *this;
}

template<typename T>
inline TAABB3D<T>& TAABB3D<T>::setMaxVertex(const TVector3<T>& maxVertex)
{
	m_maxVertex = maxVertex;

	return *this;
}

template<typename T>
inline TAABB3D<T>& TAABB3D<T>::setVertices(std::pair<TVector3<T>, TVector3<T>> minMaxVertices)
{
	setMinVertex(minMaxVertices.first);
	setMaxVertex(minMaxVertices.second);

	return *this;
}

template<typename T>
inline TAABB3D<T>& TAABB3D<T>::translate(const TVector3<T>& amount)
{
	m_minVertex.addLocal(amount);
	m_maxVertex.addLocal(amount);

	return *this;
}

template<typename T>
inline TAABB3D<T>& TAABB3D<T>::expand(const TVector3<T>& amount)
{
	m_minVertex.subLocal(amount);
	m_maxVertex.addLocal(amount);

	return *this;
}

template<typename T>
inline bool TAABB3D<T>::isPoint() const
{
	return m_minVertex.isEqual(m_maxVertex);
}

template<typename T>
inline bool TAABB3D<T>::isFiniteVolume() const
{
	const T volume = getVolume();
	return volume > T(0) && !std::isinf(volume);
}

template<typename T>
inline bool TAABB3D<T>::isValid() const
{
	return m_minVertex.x() <= m_maxVertex.x() &&
	       m_minVertex.y() <= m_maxVertex.y() &&
	       m_minVertex.z() <= m_maxVertex.z();
}

template<typename T>
inline std::pair<TAABB3D<T>, TAABB3D<T>> TAABB3D<T>::getSplitted(const constant::AxisIndexType axis, const T splitPoint) const
{
	PH_ASSERT_IN_RANGE_INCLUSIVE(splitPoint, m_minVertex[axis], m_maxVertex[axis]);

	return {TAABB3D(m_minVertex, TVector3<T>(m_maxVertex).set(axis, splitPoint)),
	        TAABB3D(TVector3<T>(m_minVertex).set(axis, splitPoint), m_maxVertex)};
}

template<typename T>
std::string TAABB3D<T>::toString() const
{
	return "min-vertex = " + m_minVertex.toString() + ", max-vertex = " + m_maxVertex.toString();
}

template<typename T>
inline bool TAABB3D<T>::isEqual(const TAABB3D& other) const
{
	return this->m_minVertex.isEqual(other.m_minVertex) && 
	       this->m_maxVertex.isEqual(other.m_maxVertex);
}

}// end namespace ph::math
