#pragma once

#include "Common/primitive_type.h"
#include "Common/config.h"
#include "Common/assertion.h"
#include "Utility/utility.h"
#include "Math/hash.h"

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
	static uint32 nextUInt32Seed();
};

template<typename T>
inline T DeterministicSeeder::nextSeed()
{
	static_assert(CHAR_BIT == 8);

	if constexpr(std::is_same_v<T, uint32>)
	{
		return nextUInt32Seed();
	}
	else if constexpr(std::is_same_v<T, uint64>)
	{
		const auto lower32 = uint64(nextUInt32Seed());
		const auto upper32 = uint64(moremur_bit_mix_64(lower32)) << 32;
		return upper32 | lower32;
	}
	else
	{
		PH_STATIC_ASSERT_DEPENDENT_FALSE(T,
			"Unsupported seed type `T`. You can provide your own implementation.");
	}
}

inline uint32 DeterministicSeeder::nextUInt32Seed()
{
	static std::atomic<uint32> seedSource(42);
#if PH_ENSURE_LOCKFREE_ALGORITHMS_ARE_LOCKLESS
	static_assert(std::atomic<uint32>::is_always_lock_free);
#endif

	return seedSource.fetch_add(1, std::memory_order_relaxed);
}

}// end namespace ph::math
