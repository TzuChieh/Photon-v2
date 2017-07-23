#pragma once

#include "Core/Bound/TAABB2D.h"

namespace ph
{

template<typename T>
inline TAABB2D<T>::TAABB2D() :
	minVertex(0), maxVertex(0)
{

}

template<typename T>
inline TAABB2D<T>::TAABB2D(const TAABB2D& other) : 
	minVertex(other.minVertex), maxVertex(other.maxVertex)
{

}

template<typename T>
inline TAABB2D<T>::TAABB2D(const TVector2<T>& minVertex, const TVector2<T>& maxVertex) : 
	minVertex(minVertex), maxVertex(maxVertex)
{

}

template<typename T>
template<typename U>
inline TAABB2D<T>::TAABB2D(const TAABB2D<U>& other) : 
	minVertex(other.minVertex), maxVertex(other.maxVertex)
{

}

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
inline T TAABB2D<T>::calcArea() const
{
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
inline bool TAABB2D<T>::isValid() const
{
	return minVertex.x <= maxVertex.x && minVertex.y <= maxVertex.y;
}

template<typename T>
inline bool TAABB2D<T>::isPoint() const
{
	return minVertex.equals(maxVertex);
}

}// end namespace ph