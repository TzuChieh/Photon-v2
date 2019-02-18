#pragma once

#include "Core/Bound/TAABB2D.h"
#include "Math/math.h"
#include "Common/assertion.h"

#include <string>

namespace ph
{

template<typename T>
inline TAABB2D<T>::TAABB2D() :
	minVertex(0), maxVertex(0)
{}

template<typename T>
inline TAABB2D<T>::TAABB2D(const TAABB2D& other) : 
	minVertex(other.minVertex), maxVertex(other.maxVertex)
{}

template<typename T>
inline TAABB2D<T>::TAABB2D(const TVector2<T>& minVertex, const TVector2<T>& maxVertex) : 
	minVertex(minVertex), maxVertex(maxVertex)
{}

template<typename T>
inline TAABB2D<T>::TAABB2D(const TVector2<T>& point) : 
	TAABB2D(point, point)
{}

template<typename T>
template<typename U>
inline TAABB2D<T>::TAABB2D(const TAABB2D<U>& other) : 
	minVertex(other.minVertex), maxVertex(other.maxVertex)
{}

template<typename T>
inline bool TAABB2D<T>::isIntersectingArea(const TAABB2D& other) const
{
	return maxVertex.x >= other.minVertex.x && minVertex.x <= other.maxVertex.x &&
	       maxVertex.y >= other.minVertex.y && minVertex.y <= other.maxVertex.y;
}

template<typename T>
inline bool TAABB2D<T>::isIntersectingArea(const TVector2<T>& point) const
{
	return point.x >= minVertex.x && point.x <= maxVertex.x && 
	       point.y >= minVertex.y && point.y <= maxVertex.y;
}

template<typename T>
inline bool TAABB2D<T>::isIntersectingRange(const TVector2<T>& point) const
{
	return point.x >= minVertex.x && point.x < maxVertex.x && 
	       point.y >= minVertex.y && point.y < maxVertex.y;
}

template<typename T>
inline T TAABB2D<T>::calcArea() const
{
	PH_ASSERT_MSG(isValid(), toString());

	return getWidth() * getHeight();
}

template<typename T>
inline TAABB2D<T>& TAABB2D<T>::unionWith(const TAABB2D& other)
{
	minVertex = minVertex.min(other.minVertex);
	maxVertex = maxVertex.max(other.maxVertex);

	return *this;
}

template<typename T>
inline TAABB2D<T>& TAABB2D<T>::intersectWith(const TAABB2D& other)
{
	minVertex = minVertex.max(other.minVertex);
	maxVertex = maxVertex.min(other.maxVertex);

	return *this;
}

template<typename T>
inline T TAABB2D<T>::getWidth() const
{
	return maxVertex.x - minVertex.x;
}

template<typename T>
inline T TAABB2D<T>::getHeight() const
{
	return maxVertex.y - minVertex.y;
}

template<typename T>
inline TVector2<T> TAABB2D<T>::getExtents() const
{
	return TVector2<T>(getWidth(), getHeight());
}

template<typename T>
inline TVector2<T> TAABB2D<T>::calcCenter() const
{
	return TVector2<T>((minVertex.x + maxVertex.x) / 2, 
	                   (minVertex.y + maxVertex.y) / 2);
}

template<typename T>
inline std::pair<TAABB2D<T>, TAABB2D<T>> TAABB2D<T>::getSplitted(const int axis, const T splitPoint) const
{
	PH_ASSERT_MSG(axis == math::X_AXIS || axis == math::Y_AXIS, std::to_string(axis));
	PH_ASSERT_IN_RANGE_INCLUSIVE(splitPoint, minVertex[axis], maxVertex[axis]);

	return {TAABB2D(minVertex, TVector2<T>(maxVertex).set(axis, splitPoint)),
	        TAABB2D(TVector2<T>(minVertex).set(axis, splitPoint), maxVertex)};
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
	return minVertex.x <= maxVertex.x && minVertex.y <= maxVertex.y;
}

template<typename T>
inline bool TAABB2D<T>::isPoint() const
{
	return minVertex.equals(maxVertex);
}

template<typename T>
inline bool TAABB2D<T>::isArea() const
{
	return maxVertex.x > minVertex.x && maxVertex.y > minVertex.y;
}

template<typename T>
inline bool TAABB2D<T>::equals(const TAABB2D& other) const
{
	return minVertex.equals(other.minVertex) && 
	       maxVertex.equals(other.maxVertex);
}

template<typename T>
inline std::string TAABB2D<T>::toString() const
{
	return "AABB: min" + minVertex.toString() + ", max" + maxVertex.toString();
}

}// end namespace ph