#pragma once

#include "Engine/Math/TVector3.h"
#include "Engine/Math/hash.h"

#include <Common/primitive_type.h>
#include <Common/utility.h>

namespace ph
{

class BsdfInputKey final
{
public:
	static BsdfInputKey makeHashed(const math::Vector3R& L, const math::Vector3R& V);
	static BsdfInputKey makeSampled(real sample);

private:
	explicit BsdfInputKey(uint32 id);

	uint32 m_id = 0;
};

inline BsdfInputKey::BsdfInputKey(uint32 id)
	: m_id{id}
{}

inline BsdfInputKey BsdfInputKey::makeHashed(const math::Vector3R& L, const math::Vector3R& V)
{
	return BsdfInputKey{
		math::combine_hashes(
			math::murmur3_32(L, 0),
			math::murmur3_32(V, 0))};
}

inline BsdfInputKey BsdfInputKey::makeSampled(real sample)
{
	return BsdfInputKey{
		bitwise_cast<uint32>(sample)};
}

}// end namespace ph
