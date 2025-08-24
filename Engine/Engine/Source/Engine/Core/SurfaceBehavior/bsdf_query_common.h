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

#include <variant>

namespace ph
{

/*! @brief Identify a set of BSDF input or value by a key value.
This class is useful when, say, you want to stochastically pick a BSDF elemental for evaluation or
to identify a BSDF query across multiple queries. Think of it like a hash value, it may have collision,
but generally well distributed.
*/
class BsdfKey final
{
public:
	static BsdfKey makeHashed(const math::Vector3R& L, const math::Vector3R& V);
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
	This is deterministic. For the same key, it will always return the same next key. If the key
	was from `makeSampled()`, the next key will decay to a hash internally.
	@param numAdvances The number of advances to perform. Must be >= 0.
	*/
	BsdfKey getNext(uint32 numAdvances) const;

private:
	struct PreHashed
	{
		uint32 value;
	};

	struct LazilyHashed
	{
		uint32 value;
	};

	struct Sampled
	{
		real value;
	};

	using Key = std::variant<std::monostate, PreHashed, LazilyHashed, Sampled>;

	BsdfKey(Key key, uint32 generation);

	Key m_key;
	uint32 m_generation;
};

inline BsdfKey::BsdfKey(Key key, uint32 generation)
	: m_key{key}
	, m_generation{generation}
{}

inline BsdfKey BsdfKey::makeHashed(const math::Vector3R& L, const math::Vector3R& V)
{
	return BsdfKey{
		PreHashed{math::murmur3_32(make_array_from_args(L, V), 0)},
		0};
}

inline BsdfKey BsdfKey::makeSampled(real sample)
{
	return BsdfKey{
		Sampled{sample},
		0};
}

inline BsdfKey BsdfKey::makeRandom()
{
	return BsdfKey{
		PreHashed{math::Random::bits32()},
		0};
}

inline uint32 BsdfKey::getValue() const
{
	switch(m_key.index())
	{
	case variant_index_of<PreHashed, Key>():
		// Caller's responsibility to ensure the key is not reused in a way that can cause correlation
		return std::get<PreHashed>(m_key).value;

	case variant_index_of<LazilyHashed, Key>():
		return math::murmur3_32(std::get<LazilyHashed>(m_key).value, m_generation);

	case variant_index_of<Sampled, Key>():
		return math::murmur3_32(std::get<Sampled>(m_key).value, m_generation);

	default:
		// Uninitialized!
		PH_ASSERT_UNREACHABLE_SECTION();
		return 0;
	}
}

inline real BsdfKey::getValueAsSample() const
{
	if(std::holds_alternative<Sampled>(m_key) && m_generation == 0)
	{
		// Caller's responsibility to ensure the key is not reused in a way that can cause correlation
		return std::get<Sampled>(m_key).value;
	}
	else
	{
		return math::bits_to_sample<real>(getValue());
	}
}

inline BsdfKey BsdfKey::getNext(uint32 numAdvances) const
{
	PH_ASSERT_GE(numAdvances, 1);

	// Keep this method cheap, as key is used rarely currently and most call sites simply call
	// this method to forward/advance the key.
	//
	// Next key must also be different to the current one while being deterministic,
	// so a consistent value can be seen across multiple call sites in different queries.
	switch(m_key.index())
	{
	case variant_index_of<PreHashed, Key>():
		// Transition to lazily hashed so no rehashing is performed until next call to `getValue()`
		return BsdfKey{
			LazilyHashed{getValue()},
			m_generation + numAdvances};

	default:
		// All other types are generation-aware, we can simply copy the key
		return BsdfKey{
			m_key,
			m_generation + numAdvances};
	}
}

}// end namespace ph
