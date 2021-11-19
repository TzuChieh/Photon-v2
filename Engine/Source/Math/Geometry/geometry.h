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
@param unitVec The unit vector to be encoded. Assumed being normalized already.
@return Encoded unit vector in [0, 1] for any bit size. 
*/
template<typename T>
inline TVector2<T> octahedron_unit_vector_encode(const TVector3<T>& unitVec)
{
	TVector3<T> octahedronProj = unitVec / unitVec.abs().sum();
	if(octahedronProj.z() < 0)
	{
		const TVector2<T> mirroredXYProj = detail::octahedron_diamond_mirror({octahedronProj.x(), octahedronProj.y()});
		octahedronProj.x() = mirroredXYProj.x();
		octahedronProj.y() = mirroredXYProj.y();
	}

	return TVector2<T>(octahedronProj.x(), octahedronProj.y()) * static_cast<T>(0.5) + static_cast<T>(0.5);
}

/*! @brief Decode and get a unit vector using octahedron projection.
@param encodedVal The encoded unit vector (in [0, 1] for any bit size) to be decoded. 
@return Decoded unit vector (normalized).
*/
template<typename T>
inline TVector3<T> octahedron_unit_vector_decode(const TVector2<T>& encodedVal)
{
	// Note that https://knarkowicz.wordpress.com/2014/04/16/octahedron-normal-vector-encoding/
	// provides an alternative approach that is faster on GPU.

	const TVector2<T> expandedEncodedVal = encodedVal * static_cast<T>(2) - static_cast<T>(1);

	TVector3<T> octahedronProj(
		expandedEncodedVal.x(),
		expandedEncodedVal.y(),
		static_cast<T>(1) - expandedEncodedVal.abs().sum());
	if(octahedronProj.z() < 0)
	{
		const TVector2<T> mirroredXYProj = detail::octahedron_diamond_mirror({octahedronProj.x(), octahedronProj.y()});
		octahedronProj.x() = mirroredXYProj.x();
		octahedronProj.y() = mirroredXYProj.y();
	}

	return octahedronProj.normalize();
}

}// end namespace ph::math
