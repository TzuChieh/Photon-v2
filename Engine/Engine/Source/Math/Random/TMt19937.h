#pragma once

#include "Math/Random/TUniformRandomBitGenerator.h"

#include <Common/primitive_type.h>

#include <random>
#include <type_traits>

namespace ph::math
{

/*! @brief Standard Mersenne Twister generator.
*/
template<typename Bits>
class TMt19937 final : public TUniformRandomBitGenerator<TMt19937<Bits>, Bits>
{
	static_assert(std::is_same_v<Bits, uint32> || std::is_same_v<Bits, uint64>,
		"Supports only `uint32` and `uint64` bits types.");

public:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(TMt19937);

	explicit TMt19937(Bits seed);

	Bits impl_generate();
	void impl_jumpAhead(uint64 distance);

private:
	// There are 32 & 64 bit versions, select the one based on `BitsT`
	using StdGeneratorType = std::conditional_t<std::is_same_v<Bits, uint32>,
		std::mt19937, std::mt19937_64>;

	StdGeneratorType m_generator;
};

template<typename Bits>
inline TMt19937<Bits>::TMt19937(const Bits seed)
	: m_generator(seed)
{}

template<typename Bits>
inline Bits TMt19937<Bits>::impl_generate()
{
	return static_cast<Bits>(m_generator());
}

template<typename Bits>
inline void TMt19937<Bits>::impl_jumpAhead(const uint64 distance)
{
	m_generator.discard(distance);
}

}// end namespace ph::math
