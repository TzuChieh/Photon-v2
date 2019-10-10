#pragma once

#include "Math/TVector3.h"
#include "Common/assertion.h"

#include <cstddef>
#include <type_traits>
#include <cmath>

namespace ph::math
{

/*
	Implements the hash function described in the paper "Optimized Spatial 
	Hashing for Collision Detection of Deformable Objects". Note that although
	they stated in the paper that 73856093, 19349663 and 83492791 are all prime
	numbers, 19349663 is in fact a composite number (41 * 471943).

	Reference: 
	http://www.beosil.com/download/CollisionDetectionHashing_VMV03.pdf
*/
template<typename Integer>
inline std::size_t discrete_spatial_hash(
	const Integer     x,
	const Integer     y,
	const Integer     z,
	const std::size_t hashTableSize)
{
	static_assert(std::is_integral_v<Integer>);

	PH_ASSERT_GT(hashTableSize, 0);

	return ((static_cast<std::size_t>(x) * 73856093) ^ 
	        (static_cast<std::size_t>(y) * 19349663) ^ 
	        (static_cast<std::size_t>(z) * 83492791)) % hashTableSize;
}

/*
	Extending the original 3-D version of discrete_spatial_hash() to 2-D.
*/
template<typename Integer>
inline std::size_t discrete_spatial_hash(
	const Integer     x,
	const Integer     y,
	const std::size_t hashTableSize)
{
	static_assert(std::is_integral_v<Integer>);

	PH_ASSERT_GT(hashTableSize, 0);

	return ((static_cast<std::size_t>(x) * 73856093) ^
	        (static_cast<std::size_t>(y) * 83492791)) % hashTableSize;
}

template<typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
inline std::size_t discrete_spatial_hash(const TVector3<T>& point, const std::size_t hashTableSize)
{
	return discrete_spatial_hash(point.x, point.y, point.z, hashTableSize);
}

/*
	Discretized spatial hash for floating point values are done by first 
	quantizing the value to integers according to cell size.
*/
template<typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
inline std::size_t discrete_spatial_hash(
	const TVector3<T>& point, 
	const TVector3<T>& cellSize,
	const std::size_t  hashTableSize)
{
	PH_ASSERT_GT(cellSize.x, 0);
	PH_ASSERT_GT(cellSize.y, 0);
	PH_ASSERT_GT(cellSize.z, 0);

	return discrete_spatial_hash(
		static_cast<std::size_t>(std::floor(point.x / cellSize.x)), 
		static_cast<std::size_t>(std::floor(point.y / cellSize.y)), 
		static_cast<std::size_t>(std::floor(point.z / cellSize.z)), 
		hashTableSize);
}

}// end namespace ph::math
