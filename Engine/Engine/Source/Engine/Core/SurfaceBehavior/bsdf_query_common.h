#pragma once

#include "Engine/Math/TVector3.h"
#include "Engine/Math/hash.h"
#include "Engine/Math/Random/Random.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Common/utility.h>

namespace ph
{

/*! @brief Identify a set of input by a key value.
Think of it like a hash value, it may have collision, but generally well distributed.
*/
class BsdfInputKey final
{
public:
	static BsdfInputKey makeHashed(const math::Vector3R& L, const math::Vector3R& V);
	static BsdfInputKey makeSampled(real sample);
	static BsdfInputKey makeRandom();

	BsdfInputKey() = default;

	/*! @brief Get the key value.
	*/
	uint32 getValue() const;

	/*! @brief Convenient method to get the key value as a sample.
	*/
	real getValueAsSample() const;

	/*! @brief Get the next key.
	*/
	BsdfInputKey getNext() const;

private:
	explicit BsdfInputKey(uint32 key);

	uint32 m_key;
#if PH_DEBUG
	bool m_hasSet{false};
#endif
};

class BsdfInputBase final
{
public:
	void set(const BsdfInputKey& key);

	const BsdfInputKey& getKey() const;

private:
	BsdfInputKey m_key;
#if PH_DEBUG
	bool m_hasSet{false};
#endif
};

inline BsdfInputKey::BsdfInputKey(uint32 key)
	: m_key{key}
#if PH_DEBUG
	// Only this ctor is a valid init
	, m_hasSet{true}
#endif
{}

inline BsdfInputKey BsdfInputKey::makeHashed(const math::Vector3R& L, const math::Vector3R& V)
{
	return BsdfInputKey{
		math::combine_hashes<uint32>(
			math::murmur3_32(L, 0),
			math::murmur3_32(V, 0))};
}

inline BsdfInputKey BsdfInputKey::makeSampled(real sample)
{
	return BsdfInputKey{math::murmur3_32(sample, 0)};
}

inline BsdfInputKey BsdfInputKey::makeRandom()
{
	return BsdfInputKey{math::Random::bits32()};
}

inline uint32 BsdfInputKey::getValue() const
{
	PH_ASSERT(m_hasSet);

	return m_key;
}

inline real BsdfInputKey::getValueAsSample() const
{

}

inline BsdfInputKey BsdfInputKey::getNext() const
{
	// Next key must be different to the current one while being deterministic,
	// so a consistent value can be seen across mutiple call sites
	return BsdfInputKey{math::murmur3_bit_mix_32(getValue())};
}

inline void BsdfInputBase::set(const BsdfInputKey& key)
{
	m_key = key;

#if PH_DEBUG
	m_hasSet = true;
#endif
}

inline const BsdfInputKey& BsdfInputBase::getKey() const
{
	PH_ASSERT(m_hasSet);

	return m_key;
}

}// end namespace ph
