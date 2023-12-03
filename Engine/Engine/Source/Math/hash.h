#pragma once

#include "Math/math_fwd.h"
#include "Common/primitive_type.h"

#include <cstddef>
#include <concepts>

namespace ph::math
{

/*!
Implements the hash function described in the paper "Optimized Spatial 
Hashing for Collision Detection of Deformable Objects". Note that although
they stated in the paper that 73856093, 19349663 and 83492791 are all prime
numbers, 19349663 is in fact a composite number (41 * 471943).

Reference: 
http://www.beosil.com/download/CollisionDetectionHashing_VMV03.pdf
*/
template<typename Integer>
std::size_t discrete_spatial_hash(
	Integer x,
	Integer y,
	Integer z,
	std::size_t hashTableSize);

/*!
Extending the original 3-D version of discrete_spatial_hash() to 2-D.
*/
template<typename Integer>
std::size_t discrete_spatial_hash(
	Integer x,
	Integer y,
	std::size_t hashTableSize);

template<std::integral T>
std::size_t discrete_spatial_hash(const TVector3<T>& point, std::size_t hashTableSize);

/*!
Discretized spatial hash for floating point values are done by first 
quantizing the value to integers according to cell size.
*/
template<std::floating_point T>
std::size_t discrete_spatial_hash(
	const TVector3<T>& point, 
	const TVector3<T>& cellSize,
	std::size_t hashTableSize);

/*! @brief MurmurHash3's bit mixer.
32-bit version.
*/
uint32 murmur3_bit_mix_32(uint32 v);

/*! @brief MurmurHash3's bit mixer.
64-bit version.
*/
uint64 murmur3_bit_mix_64(uint64 v);

/*! @brief MurmurHash3's bit mixer.
64-bit version.
*/
uint64 murmur3_v13_bit_mix_64(uint64 v);

/*! @brief A MurmurHash3-style bit mixer that outperforms the original by quite some margin.
64-bit version.
*/
uint64 moremur_bit_mix_64(uint64 v);

/*! @brief Generate 32-bit hash values using MurmurHash3.
Note that there are no collisions when `T` has <= 32 bits (two distinct inputs will not result in the
same output). In general, if there is a way to reverse a function (reversing `murmur3_32()` is possible), 
it is guaranteed there cannot be any collisions. Otherwise, it would not be possible to revert.
*/
template<typename T>
uint32 murmur3_32(const T& data, uint32 seed);

/*! @brief Generate 32-bit hash values using MurmurHash3.
@param data Pointer to a data array of type `T`.
@param dataSize Number of elements in the data array.
@param seed A value for generating the hash. Can be from a RNG or anything you prefer.
*/
template<typename T>
uint32 murmur3_32(const T* data, std::size_t dataSize, uint32 seed);

}// end namespace ph::math

#include "Math/hash.ipp"
