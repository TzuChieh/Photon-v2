#pragma once

#include "Math/Geometry/TAABB3D.h"

#include <Common/assertion.h>

#include <algorithm>
#include <cmath>
#include <type_traits>

namespace ph::math
{

template<typename T>
inline TAABB3D<T> TAABB3D<T>::makeEmpty()
{
	constexpr T maxVal = std::numeric_limits<T>::max();
	constexpr T minVal = std::numeric_limits<T>::lowest();

	return TAABB3D(
		{maxVal, maxVal, maxVal},
		{minVal, minVal, minVal});
}

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
{}

template<typename T>
template<bool IS_ROBUST>
inline bool TAABB3D<T>::isIntersectingVolume(const TLineSegment<T>& segment) const
{
	T nearHitT, farHitT;
	return isIntersectingVolume<IS_ROBUST>(segment, &nearHitT, &farHitT);
}

template<typename T>
template<bool IS_ROBUST>
inline bool TAABB3D<T>::isIntersectingVolume(
	const TLineSegment<T>& segment,
	T* const               out_nearHitT,
	T* const               out_farHitT) const
{
	return isIntersectingVolume<IS_ROBUST>(segment, segment.getDir().rcp(), out_nearHitT, out_farHitT);
}

template<typename T>
template<bool IS_ROBUST>
inline bool TAABB3D<T>::isIntersectingVolume(
	const TLineSegment<T>& segment,
	const TVector3<T>& rcpSegmentDir,
	T* const out_nearHitT,
	T* const out_farHitT) const
{
	PH_ASSERT(out_nearHitT);
	PH_ASSERT(out_farHitT);

	const auto [tMin, tMax] = isIntersectingVolume<IS_ROBUST>(segment, rcpSegmentDir);

	*out_nearHitT = tMin;
	*out_farHitT  = tMax;

	return tMin <= tMax;
}

template<typename T>
template<bool IS_ROBUST>
inline std::pair<T, T> TAABB3D<T>::isIntersectingVolume(
	const TLineSegment<T>& segment,
	const TVector3<T>& rcpSegmentDir) const
{
	if constexpr(IS_ROBUST)
	{
		return intersectVolumeRobust(segment, rcpSegmentDir);
	}
	else
	{
		//return intersectVolumeKajiyaKay(segment, rcpSegmentDir);
		return intersectVolumeTavian(segment, rcpSegmentDir);
	}
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
inline bool TAABB3D<T>::isEmpty() const
{
	return m_minVertex.x() > m_maxVertex.x() ||
	       m_minVertex.y() > m_maxVertex.y() ||
	       m_minVertex.z() > m_maxVertex.z();
}

template<typename T>
inline bool TAABB3D<T>::isPoint() const
{
	return m_minVertex.isEqual(m_maxVertex);
}

template<typename T>
inline bool TAABB3D<T>::isVolume() const
{
	return m_minVertex.x() < m_maxVertex.x() &&
	       m_minVertex.y() < m_maxVertex.y() &&
	       m_minVertex.z() < m_maxVertex.z();
}

template<typename T>
inline bool TAABB3D<T>::isFiniteVolume() const
{
	return isVolume() && std::isfinite(getVolume());
}

template<typename T>
inline TAABB3D<T> TAABB3D<T>::getTranslated(const TVector3<T>& amount) const
{
	return TAABB3D(*this).translate(amount);
}

template<typename T>
inline std::pair<TAABB3D<T>, TAABB3D<T>> TAABB3D<T>::getSplitted(const std::size_t axis, const T splitPoint) const
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

template<typename T>
inline std::pair<T, T> TAABB3D<T>::intersectVolumeKajiyaKay(
	const TLineSegment<T>& segment,
	const TVector3<T>& rcpSegmentDir) const
{
	PH_ASSERT(!std::isnan(segment.getMinT()) && !std::isnan(segment.getMaxT()));

	// The starting ray interval (tMin, tMax) will be incrementally intersect
	// against each ray-slab hitting interval (t1, t2) and be updated with the
	// resulting interval.
	//
	// Note that the following implementation is NaN-aware 
	// (tMin/tMax will never be NaN as long as the parametric range of the segment contains no NaN)

	T tMin = segment.getMinT();
	T tMax = segment.getMaxT();

	// Find ray-slab hitting interval in x-axis then intersect with (tMin, tMax)

	T t1 = (m_minVertex.x() - segment.getOrigin().x()) * rcpSegmentDir.x();
	T t2 = (m_maxVertex.x() - segment.getOrigin().x()) * rcpSegmentDir.x();

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
		return {tMin, tMax};
	}

	// Find ray-slab hitting interval in y-axis then intersect with (tMin, tMax)

	t1 = (m_minVertex.y() - segment.getOrigin().y()) * rcpSegmentDir.y();
	t2 = (m_maxVertex.y() - segment.getOrigin().y()) * rcpSegmentDir.y();

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
		return {tMin, tMax};
	}

	// Find ray-slab hitting interval in z-axis then intersect with (tMin, tMax)

	t1 = (m_minVertex.z() - segment.getOrigin().z()) * rcpSegmentDir.z();
	t2 = (m_maxVertex.z() - segment.getOrigin().z()) * rcpSegmentDir.z();

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

	return {tMin, tMax};
}

template<typename T>
inline std::pair<T, T> TAABB3D<T>::intersectVolumeTavian(
	const TLineSegment<T>& segment,
	const TVector3<T>& rcpSegmentDir) const
{
	PH_ASSERT(!std::isnan(segment.getMinT()) && !std::isnan(segment.getMaxT()));

	T tMin = segment.getMinT();
	T tMax = segment.getMaxT();

	// Find ray-slab hitting interval in the i-th dimension then intersect with (tMin, tMax)
	for(std::size_t i = 0; i < 3; ++i)
	{
		const T t1 = (m_minVertex[i] - segment.getOrigin()[i]) * rcpSegmentDir[i];
		const T t2 = (m_maxVertex[i] - segment.getOrigin()[i]) * rcpSegmentDir[i];

		tMin = std::max(tMin, std::min(t1, t2));
		tMax = std::min(tMax, std::max(t1, t2));
	}

	return {tMin, tMax};
}

template<typename T>
inline std::pair<T, T> TAABB3D<T>::intersectVolumeRobust(
	const TLineSegment<T>& segment,
	const TVector3<T>& rcpSegmentDir) const
{
	PH_ASSERT(!std::isnan(segment.getMinT()) && !std::isnan(segment.getMaxT()));

	T tMin = segment.getMinT();
	T tMax = segment.getMaxT();

	// Find ray-slab hitting interval in the i-th dimension then intersect with (tMin, tMax)
	for(std::size_t i = 0; i < 3; ++i)
	{
		const T minMaxSlabDist[2] = {
			m_minVertex[i] - segment.getOrigin()[i],
			m_maxVertex[i] - segment.getOrigin()[i]};

		const bool isNegDir = segment.getDir()[i] < 0;
		const T    minDist  = minMaxSlabDist[    isNegDir] * rcpSegmentDir[i];
		const T    maxDist  = minMaxSlabDist[1 - isNegDir] * rcpSegmentDir[i];

		tMin = minDist > tMin ? minDist : tMin;// safe max: fallback to `tMin` in case of NaN
		tMax = maxDist < tMax ? maxDist : tMax;// safe min: fallback to `tMax` in case of NaN
	}

	// C++ defined `epsilon()` as the interval machine epsilon (e_i), while the paper defined
	// the epsilon as rounding machine epsilon (e_r). The relation between them is "2 * e_r = e_i".
	constexpr T multiplier = std::numeric_limits<T>::epsilon() * 2 + 1;

	return {tMin, tMax * multiplier};
}

}// end namespace ph::math
