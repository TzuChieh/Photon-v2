#pragma once

#include "Math/hash.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"

#include <type_traits>
#include <cmath>
#include <bit>
#include <algorithm>
#include <climits>

namespace ph::math
{

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

template<std::integral T>
inline std::size_t discrete_spatial_hash(const TVector3<T>& point, const std::size_t hashTableSize)
{
	return discrete_spatial_hash(point.x, point.y, point.z, hashTableSize);
}

template<std::floating_point T>
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

inline uint32 murmur3_bit_mix_32(uint32 v)
{
	v ^= v >> 16;
	v *= 0x85EBCA6BUL;
	v ^= v >> 13;
	v *= 0xC2B2AE35UL;
	v ^= v >> 16;

	return v;
}

inline uint64 murmur3_bit_mix_64(uint64 v)
{
	v ^= (v >> 33);
	v *= 0xFF51AFD7ED558CCDULL;
	v ^= (v >> 33);
	v *= 0xC4CEB9FE1A85EC53ULL;
	v ^= (v >> 33);

	return v;
}

inline uint64 murmur3_v13_bit_mix_64(uint64 v)
{
	v ^= (v >> 30);
	v *= 0xBF58476D1CE4E5B9ULL;
	v ^= (v >> 27);
	v *= 0x94D049BB133111EBULL;
	v ^= (v >> 31);

	return v;
}

inline uint64 moremur_bit_mix_64(uint64 v)
{
	// The constants were derived by Pelle Evensen:
	// https://mostlymangling.blogspot.com/2019/12/stronger-better-morer-moremur-better.html

	v ^= v >> 27;
	v *= 0x3C79AC492BA7B653ULL;
	v ^= v >> 33;
	v *= 0x1C69B3F74AC4AE35ULL;
	v ^= v >> 27;

	return v;
}

template<typename T>
inline uint32 murmur3_32(const T& data, const uint32 seed)
{
	return murmur3_32(&data, 1, seed);
}

template<typename T>
inline uint32 murmur3_32(const T* const data, const std::size_t dataSize, const uint32 seed)
{
	/*
	References:
	[1] Wiki: https://en.wikipedia.org/wiki/MurmurHash (`murmur3_32()`)
	[2] aappleby's smhasher: https://github.com/aappleby/smhasher/ (`MurmurHash3_x86_32()`)
	*/

	static_assert(CHAR_BIT == 8);
	static_assert(std::is_trivially_copyable_v<T>,
		"`T` should be trivially copyable to be able to interpret it as bytes.");

	constexpr uint32 c1 = 0xCC9E2D51UL;
	constexpr uint32 c2 = 0x1B873593UL;
	constexpr int r1 = 15;
	constexpr int r2 = 13;
	constexpr uint32 m = 5;
	constexpr uint32 n = 0xE6546B64UL;

	auto const bytes = reinterpret_cast<const uint8*>(data);
	const std::size_t numBytes = dataSize * sizeof(T);
	const std::size_t numBlocks = numBytes / 4;

	uint32 h1 = seed;

	// Body

	// Read in blocks of 4 bytes
	for(uint32 bi = 0; bi < numBlocks; ++bi)
	{
		const auto byteIndex = bi * 4;
		PH_ASSERT_LT(byteIndex, numBytes);

		uint32 block32;
		std::copy_n(bytes + byteIndex, 4, reinterpret_cast<uint8*>(&block32));

		// To remove a source of differing results across endiannesses, perform byte swap on
		// big-endian CPUs (a swap here has no effects on hash properties though)
		if constexpr(std::endian::native == std::endian::big)
		{
			block32 = std::byteswap(block32);
		}

		uint32 k1 = block32;

		k1 *= c1;
		k1 = std::rotl(k1, r1);
		k1 *= c2;

		h1 ^= k1;
		h1 = std::rotl(h1, r2);
		h1 = h1 * m + n;
	}

	// Tail

	auto const tailBytes = bytes + numBlocks * 4;

	uint32 k1 = 0;

	switch(numBytes & 3)
	{
	case 3: k1 ^= tailBytes[2] << 16UL;
	        [[fallthrough]];
	case 2: k1 ^= tailBytes[1] << 8UL;
	        [[fallthrough]];
	case 1: k1 ^= tailBytes[0];
	        k1 *= c1; k1 = std::rotl(k1, r1); k1 *= c2; h1 ^= k1;
	};

	// Finalization

	h1 ^= static_cast<uint32>(dataSize);
	h1 = murmur3_bit_mix_32(h1);
	return h1;
}

}// end namespace ph::math
