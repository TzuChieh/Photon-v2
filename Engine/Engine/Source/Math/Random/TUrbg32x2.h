#pragma once

#include "Math/Random/TUniformRandomBitGenerator.h"

#include <Common/primitive_type.h>

#include <type_traits>
#include <utility>

namespace ph::math
{

/*! @brief Combining two 32-bit RNGs to form a new 64-bit RNG.
If any of the input RNG is not a 32-bit type, this generator will treat it as a 32-bit RNG
(e.g., by converting its generated bits to 32-bit).
*/
template<CURBG URBG1, CURBG URBG2 = URBG1>
class TUrbg32x2 final : public TUniformRandomBitGenerator<TUrbg32x2<URBG1, URBG2>, uint64>
{
	static_assert(
		std::is_same_v<typename URBG1::BitsType, uint32> || 
		std::is_same_v<typename URBG1::BitsType, uint64>,
		"URBG1 must have `uint32` or `uint64` bits type.");

	static_assert(
		std::is_same_v<typename URBG2::BitsType, uint32> ||
		std::is_same_v<typename URBG2::BitsType, uint64>,
		"URBG2 must have `uint32` or `uint64` bits type.");

public:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(TUrbg32x2);

	TUrbg32x2(URBG1 urbg1, URBG2 urbg2);

	uint64 impl_generate();
	void impl_jumpAhead(uint64 distance);

private:
	URBG1 m_urbg1;
	URBG2 m_urbg2;
};

template<CURBG URBG1, CURBG URBG2>
inline TUrbg32x2<URBG1, URBG2>::TUrbg32x2(URBG1 urbg1, URBG2 urbg2)
	: m_urbg1(std::move(urbg1))
	, m_urbg2(std::move(urbg2))
{}

template<CURBG URBG1, CURBG URBG2>
inline uint64 TUrbg32x2<URBG1, URBG2>::impl_generate()
{
	// Generate 8-byte bits by combining two 4-byte bits
	// (prefer `URGB1` by placing it in the higher bits, since some algorithm consider higher bits
	// may be of better quality by default)
	const auto higher4B = uint64(m_urbg1.template generate<uint32>()) << 32;
	const auto lower4B = uint64(m_urbg2.template generate<uint32>());
	return higher4B | lower4B;
}

template<CURBG URBG1, CURBG URBG2>
inline void TUrbg32x2<URBG1, URBG2>::impl_jumpAhead(const uint64 distance)
{
	m_urbg1.jumpAhead(distance);
	m_urbg2.jumpAhead(distance);
}

}// end namespace ph::math
