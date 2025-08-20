#pragma once

#include "Engine/Core/SurfaceBehavior/bsdf_query_fwd.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Math/hash.h"
#include "Engine/Math/Random/sample.h"
#include "Engine/Math/Random/Random.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Common/utility.h>

#include <variant>

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
	using Key = std::variant<std::monostate, uint32, real>;

	explicit BsdfInputKey(Key key);

	// TODO: implement generation for cheaper `getNext()`
	// TODO: move to ctx

	Key m_key;
	uint32 m_generation;
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

inline BsdfInputKey::BsdfInputKey(Key key)
	: m_key{key}
{}

inline BsdfInputKey BsdfInputKey::makeHashed(const math::Vector3R& L, const math::Vector3R& V)
{
	return BsdfInputKey{math::murmur3_32(make_array_from_args(L, V), 0)};
}

inline BsdfInputKey BsdfInputKey::makeSampled(real sample)
{
	return BsdfInputKey{sample};
}

inline BsdfInputKey BsdfInputKey::makeRandom()
{
	return BsdfInputKey{math::Random::bits32()};
}

inline uint32 BsdfInputKey::getValue() const
{
	PH_ASSERT(!std::holds_alternative<std::monostate>(m_key));

	return std::holds_alternative<uint32>(m_key)
		? std::get<uint32>(m_key)
		: math::murmur3_32(std::get<real>(m_key), 0);
}

inline real BsdfInputKey::getValueAsSample() const
{
	PH_ASSERT(!std::holds_alternative<std::monostate>(m_key));

	return std::holds_alternative<uint32>(m_key)
		? math::bits_to_sample<real>(std::get<uint32>(m_key))
		: std::get<real>(m_key);
}

inline BsdfInputKey BsdfInputKey::getNext() const
{
	PH_ASSERT(!std::holds_alternative<std::monostate>(m_key));

	// Next key must be different to the current one while being deterministic,
	// so a consistent value can be seen across mutiple call sites
	return BsdfInputKey{math::murmur3_32(m_key, 0)};
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
