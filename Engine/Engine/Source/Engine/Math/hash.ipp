#pragma once

#include "Engine/Math/hash.h"
#include "Engine/Math/Random/sample.h"

#include <Common/assertion.h>

#include <type_traits>
#include <cmath>
#include <bit>
#include <algorithm>
#include <climits>
#include <utility>

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

/*
Algorithm provenance:
- Original: Bob Jenkins, `lookup3.c`, functions `mix()`, `final()`, and `hashword()`:
  http://burtleburtle.net/bob/c/lookup3.c
- Reference implementation: Blender Cycles `src/util/hash.h`, functions `mix`, `final`, `hash_uint`,
  `hash_uint2`, `hash_uint3`, and `hash_uint4`:
  https://github.com/blender/cycles/blob/97dbe6f57cdf4ede2d2b75ebdda507c8712edb7a/src/util/hash.h#L78-L184
- Conversion reference: Blender Cycles `src/util/hash.h`, functions `hash_float_to_float` and
  `hash_float2_to_float`:
  https://github.com/blender/cycles/blob/97dbe6f57cdf4ede2d2b75ebdda507c8712edb7a/src/util/hash.h#L217-L225
*/
namespace detail::jenkins
{

inline void finalize_3(uint32& a, uint32& b, uint32& c)
{
	c ^= b;
	c -= std::rotl(b, 14);
	a ^= c;
	a -= std::rotl(c, 11);
	b ^= a;
	b -= std::rotl(a, 25);
	c ^= b;
	c -= std::rotl(b, 16);
	a ^= c;
	a -= std::rotl(c, 4);
	b ^= a;
	b -= std::rotl(a, 14);
	c ^= b;
	c -= std::rotl(b, 24);
}

inline void mix_3(uint32& a, uint32& b, uint32& c)
{
	a -= c;
	a ^= std::rotl(c, 4);
	c += b;
	b -= a;
	b ^= std::rotl(a, 6);
	a += c;
	c -= b;
	c ^= std::rotl(b, 8);
	b += a;
	a -= c;
	a ^= std::rotl(c, 16);
	c += b;
	b -= a;
	b ^= std::rotl(a, 19);
	a += c;
	c -= b;
	c ^= std::rotl(b, 4);
	b += a;
}

}// end namespace detail::jenkins

inline uint32 jenkins_lookup3_32(const uint32 x)
{
	uint32 a = 0xDEADBEEFU + (1U << 2U) + 13U;
	uint32 b = a;
	uint32 c = a;
	a += x;
	detail::jenkins::finalize_3(a, b, c);
	return c;
}

inline uint32 jenkins_lookup3_32(const TSpanView<uint32, 2> words)
{
	uint32 a = 0xDEADBEEFU + (2U << 2U) + 13U;
	uint32 b = a;
	uint32 c = a;
	a += words[0];
	b += words[1];
	detail::jenkins::finalize_3(a, b, c);
	return c;
}

inline uint32 jenkins_lookup3_32(const TSpanView<uint32, 3> words)
{
	uint32 a = 0xDEADBEEFU + (3U << 2U) + 13U;
	uint32 b = a;
	uint32 c = a;
	a += words[0];
	b += words[1];
	c += words[2];
	detail::jenkins::finalize_3(a, b, c);
	return c;
}

inline uint32 jenkins_lookup3_32(const TSpanView<uint32, 4> words)
{
	uint32 a = 0xDEADBEEFU + (4U << 2U) + 13U;
	uint32 b = a;
	uint32 c = a;
	a += words[0];
	b += words[1];
	c += words[2];
	detail::jenkins::mix_3(a, b, c);
	a += words[3];
	detail::jenkins::finalize_3(a, b, c);
	return c;
}

inline float32 jenkins_lookup3_to_unit(const float32 seed)
{
	return bits_to_sample<float32>(jenkins_lookup3_32(std::bit_cast<uint32>(seed)));
}

inline float32 jenkins_lookup3_to_unit(const TSpanView<float32, 2> seeds)
{
	const uint32 words[] = {
		std::bit_cast<uint32>(seeds[0]),
		std::bit_cast<uint32>(seeds[1])};
	return bits_to_sample<float32>(jenkins_lookup3_32(words));
}

inline uint32 murmur3_bit_mix_32(uint32 v)
{
	v ^= (v >> 16);
	v *= 0x85EBCA6BUL;
	v ^= (v >> 13);
	v *= 0xC2B2AE35UL;
	v ^= (v >> 16);

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

	v ^= (v >> 27);
	v *= 0x3C79AC492BA7B653ULL;
	v ^= (v >> 33);
	v *= 0x1C69B3F74AC4AE35ULL;
	v ^= (v >> 27);

	return v;
}

template<typename T, typename BitMixerType>
inline uint32 murmur3_32(const T& data, const uint32 seed, BitMixerType&& bitMixer)
{
	return murmur3_32(&data, 1, seed, std::forward<BitMixerType>(bitMixer));
}

template<typename T, typename BitMixerType>
inline uint32 murmur3_32(
	const T* const data,
	const std::size_t dataSize,
	const uint32 seed,
	BitMixerType&& bitMixer)
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
	h1 = std::forward<BitMixerType>(bitMixer)(h1);
	return h1;
}

inline uint32 permuted_index(uint32 i, uint32 l, uint32 p)
{
	// This is the implementation from Kensler's paper: "Correlated Multi-Jittered Sampling".
	// See https://afnan.io/posts/2019-04-05-explaining-the-hashed-permutation/ for
	// a nice introduction.
	// Note that PBRT-v4 also uses the same implementation, see their `PermutationElement()`.

	unsigned w = l - 1;
	w |= w >> 1;
	w |= w >> 2;
	w |= w >> 4;
	w |= w >> 8;
	w |= w >> 16;

	do
	{
		i ^= p;
		i *= 0xe170893d;
		i ^= p >> 16;
		i ^= (i & w) >> 4;
		i ^= p >> 8;
		i *= 0x0929eb3f;
		i ^= p >> 23;
		i ^= (i & w) >> 1;
		i *= 1 | p >> 27;
		i *= 0x6935fa69;
		i ^= (i & w) >> 11;
		i *= 0x74dcb303;
		i ^= (i & w) >> 2;
		i *= 0x9e501cc3;
		i ^= (i & w) >> 2;
		i *= 0xc860a3df;
		i &= w;
		i ^= i >> 5;
	} while (i >= l);

	return (i + p) % l; 
}

template<std::integral T>
inline T combine_hashes(const T& hashA, const T& hashB)
{
	using UnsignedT = std::make_unsigned_t<T>;

	const UnsignedT uHashA(hashA);
	const UnsignedT uHashB(hashB);

	auto newHash = uHashA;
	if constexpr(sizeof(UnsignedT) <= 2)
	{
		newHash ^= uHashB + UnsignedT(0x9e37U) + (uHashA << 3) + (uHashA >> 1);
	}
	else if constexpr(sizeof(UnsignedT) <= 4)
	{
		newHash ^= uHashB + UnsignedT(0x9e3779b9U) + (uHashA << 6) + (uHashA >> 2);
	}
	else
	{
		// For 64-bit; 128-bit would be 0x9e3779b97f4a7c15f39cc0605d396154
		newHash ^= uHashB + UnsignedT(0x9e3779b97f4a7c15LLU) + (uHashA << 12) + (uHashA >> 4);
	}
	return T(newHash);
}

}// end namespace ph::math
