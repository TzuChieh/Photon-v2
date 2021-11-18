#pragma once

#include "Math/TVector2.h"

namespace ph::math
{

namespace detail
{

/*!
Consider an unwrapped octahedron (onto a square in [-1, 1]), with upper faces form a diamond shape 
in the middle, and the lower faces fill the remaining triangular areas. The function will map points 
in a triangle to the other triangle in the same quadrant, mirrored along their common edge.
*/
template<typename T>
TVector2<T> octahedron_diamond_mirror(const TVector2<T>& point)
{
	const TVector2<T> diagonallyMirroredPoint(point.y(), point.x());

	// Basically mirror and flip along various axes
	return diagonallyMirroredPoint.abs().complement() * 
	       TVector2<T>(point.x() >= 0 ? 1 : -1, point.y() >= 0 ? 1 : -1);
}

}// end namespace detail

}// end namespace ph::math
