#pragma once

#include "Math/TVector2.h"
#include "Math/TVector3.h"

namespace ph::math
{

namespace detail
{

/*!
Consider an unwrapped octahedron (onto a square in [-1, 1]), with upper faces form a diamond shape 
in the middle, and the lower faces fill the remaining triangular areas. The function will map points 
in a triangle to the other triangle in the same quadrant, mirrored along their common edge.

[1] https://knarkowicz.wordpress.com/2014/04/16/octahedron-normal-vector-encoding/
[2] A Survey of Efficient Representations for Independent Unit Vectors 
    https://jcgt.org/published/0003/02/01/paper.pdf
    (see supplemental materials for C++ reference implmentation)
*/
template<typename T>
inline TVector2<T> octahedron_diamond_mirror(const TVector2<T>& point)
{
	const TVector2<T> diagonallyMirroredPoint(point.y(), point.x());

	/*
	Note that in [1], it is calculated as

		return (1.0 - abs(v.yx)) * (v.xy >= 0.0 ? 1.0 : -1.0);

	which translate to the following:
	*/
	// Basically mirror and flip along various axes
	return diagonallyMirroredPoint.abs().complement() * 
	       TVector2<T>(point.x() >= 0 ? 1 : -1, point.y() >= 0 ? 1 : -1);

	/*
	However, that seems overly complexed. Here I derived a simpler equivalent. It is worth noting that
	my variant is likely inferior as the paper [2] uses the same formulation as in [1]. They use only
	sign changes and complements to achieve the mirror, and should be robust (no seams due to quantization
	errors; mine's uses arithmetic on the coordinates which can cause problems I guess). 
	*/
	/*return diagonallyMirroredPoint + 
	       TVector2<T>(point.x() >= 0 ? 1 : -1, point.y() >= 0 ? 1 : -1);*/
}

}// end namespace detail

// TODO: precise version of the encoding (see [2])

/*! @brief Encode a unit vector using octahedron projection.
@return Encoded unit vector in [-1, 1] for any bit size. 
*/
//template<typename T>
//inline TVector2<T> octahedron_unit_vector_encode(const TVector3<T>& vec)
//{
//	const TVector3<T> octahedronProj = vec.abs().
//}

}// end namespace ph::math
