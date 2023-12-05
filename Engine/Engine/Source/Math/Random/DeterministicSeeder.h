#pragma once

#include "Utility/utility.h"
#include "Math/hash.h"

#include <Common/assertion.h>
#include <Common/config.h>
#include <Common/primitive_type.h>

#include <atomic>
#include <climits>
#include <type_traits>

namespace ph::math
{

/*! @brief Convenient thread-safe seed provider for RNGs.
Do not use this for cryptography.
*/
class DeterministicSeeder final
{
public:
	template<typename T>
	static T nextSeed();

private:
	static uint32 nextUInt32Number();
};

template<typename T>
inline T DeterministicSeeder::nextSeed()
{
	static_assert(CHAR_BIT == 8);

	// The goal here is to generate values that will not repeat themselves unless this method is
	// called a large amount of times (e.g., more than 2^32 times), and the values should look
	// uncorrelated. MurmurHash3 generates unique hash values for <= 32-bit inputs and is used
	// here (seed values used here are randomly chosen prime numbers).

	const auto number = nextUInt32Number();
	if constexpr(std::is_same_v<T, uint32>)
	{
		return murmur3_32(number, 1236161);
	}
	else if constexpr(std::is_same_v<T, uint64>)
	{
		const auto lower32 = uint64(murmur3_32(number, 2237617));
		const auto upper32 = uint64(murmur3_32(number, 3237557)) << 32;
		return upper32 | lower32;
	}
	else
	{
		PH_STATIC_ASSERT_DEPENDENT_FALSE(T,
			"Unsupported seed type `T`. You can provide your own implementation.");
	}
}

inline uint32 DeterministicSeeder::nextUInt32Number()
{
	static std::atomic<uint32> numberSource(42);
#if PH_ENSURE_LOCKFREE_ALGORITHMS_ARE_LOCKLESS
	static_assert(std::atomic<uint32>::is_always_lock_free);
#endif

	return numberSource.fetch_add(1, std::memory_order_relaxed);
}

}// end namespace ph::math
