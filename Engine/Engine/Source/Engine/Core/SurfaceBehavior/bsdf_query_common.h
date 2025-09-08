#pragma once

#include "Engine/Core/SurfaceBehavior/bsdf_query_fwd.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Math/hash.h"
#include "Engine/Math/Random/sample.h"
#include "Engine/Math/Random/Random.h"
#include "Engine/Utility/utility.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Common/utility.h>

#include <utility>
#include <variant>

namespace ph
{

/*! @brief Identify a target BSDF by a key value.
This class is useful when you want to identify a target BSDF across multiple queries.
Think of it like a hash value, it may have collision, but generally well distributed.
For BSDF implementations, an example usage is using this key to stochastically pick
a BSDF elemental for evaluation.
*/
class BsdfKey final
{
public:
	template<typename Arg, typename... Args>
	static BsdfKey makeHashed(Arg&& hashTarget, Args&&... moreHashTargets);

	static BsdfKey makeSampled(real sample);
	static BsdfKey makeRandom();

	BsdfKey() = default;

	/*! @brief Get the key value.
	*/
	uint32 getValue() const;

	/*! @brief Convenient method to get the key value as a sample.
	*/
	real getValueAsSample() const;

	/*! @brief Get the next key derived from this key.
	This is deterministic. For the same key and same hash targets, it will always return the same
	next key. If the key was from `makeSampled()`, the next key will decay to a hash internally.
	*/
	template<typename Arg, typename... Args>
	BsdfKey getNext(Arg&& hashTarget, Args&&... moreHashTargets) const;

private:
	struct Hashed
	{
		uint32 value;
	};

	struct Sampled
	{
		real value;
	};

	using Key = std::variant<std::monostate, Hashed, Sampled>;

	// Seed for hashing. This seed is randomly chosen, so it is unlikely to correlate with other
	// hashes in the program (for example, we may not want to use 0, as it is common).
	inline constexpr static uint32 SEED = 0xA566FC83;

	explicit BsdfKey(Key key);

	Key m_key;
};

inline BsdfKey::BsdfKey(Key key)
	: m_key{key}
{}

template<typename Arg, typename... Args>
inline BsdfKey BsdfKey::makeHashed(Arg&& hashTarget, Args&&... moreHashTargets)
{
	return BsdfKey{
		Hashed{
			math::murmur3_32(
				make_array_from_args(
					std::forward<Arg>(hashTarget),
					std::forward<Args>(moreHashTargets)...),
				SEED)}};
}

inline BsdfKey BsdfKey::makeSampled(real sample)
{
	return BsdfKey{
		Sampled{sample}};
}

inline BsdfKey BsdfKey::makeRandom()
{
	return BsdfKey{
		Hashed{math::Random::bits32()}};
}

inline uint32 BsdfKey::getValue() const
{
	switch(m_key.index())
	{
	case variant_index_of<Hashed, Key>():
		// Caller's responsibility to ensure the key is not reused in a way that can cause correlation
		return std::get<Hashed>(m_key).value;

	case variant_index_of<Sampled, Key>():
		return math::murmur3_32(std::get<Sampled>(m_key).value, SEED);

	default:
		// Uninitialized!
		PH_ASSERT_UNREACHABLE_SECTION();
		return 0xDEADBEEF;
	}
}

inline real BsdfKey::getValueAsSample() const
{
	if(std::holds_alternative<Sampled>(m_key))
	{
		// Caller's responsibility to ensure the key is not reused in a way that can cause correlation
		return std::get<Sampled>(m_key).value;
	}
	else
	{
		return math::bits_to_sample<real>(getValue());
	}
}

template<typename Arg, typename... Args>
inline BsdfKey BsdfKey::getNext(Arg&& hashTarget, Args&&... moreHashTargets) const
{
	// Mix with current value since next key must be different to the current one
	// while being deterministic
	const uint32 current = getValue();

	return BsdfKey{
		Hashed{
			math::combine_hashes(
				current,
				math::murmur3_32(
					make_array_from_args(
						std::forward<Arg>(hashTarget),
						std::forward<Args>(moreHashTargets)...),
					SEED))}};
}

}// end namespace ph
