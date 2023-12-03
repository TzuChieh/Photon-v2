#pragma once

#include "Math/Random/TUniformRandomBitGenerator.h"
#include "Common/primitive_type.h"
#include "Math/hash.h"
#include "Math/math.h"

// This is a good reference on built-in 128-bit integer:
// https://stackoverflow.com/questions/16088282/is-there-a-128-bit-integer-in-gcc
// In the future, consider using `_BitInt`

/*! Define as 1 to force the use of emulated 128-bit math. */
#define PH_MATH_PCG64_FORCE_EMULATED_UINT128 0

#if !defined(__SIZEOF_INT128__) || PH_MATH_PCG64_FORCE_EMULATED_UINT128
#define PH_MATH_PCG64_EMULATED_UINT128 1
#else
#define PH_MATH_PCG64_EMULATED_UINT128 0
#endif

namespace ph::math
{

namespace detail
{

/*! Minimal 128-bit math for PCG-64. */
class Pcg64UInt128
{
public:
	constexpr Pcg64UInt128();
	constexpr Pcg64UInt128(uint64 high64, uint64 low64);

	uint64 getHigh64() const;
	uint64 getLow64() const;

	Pcg64UInt128 operator + (const Pcg64UInt128& rhs) const;
	Pcg64UInt128 operator * (const Pcg64UInt128& rhs) const;

private:
#if PH_MATH_PCG64_EMULATED_UINT128
	uint64 m_high64;
	uint64 m_low64;
#else
	__uint128_t m_128;
#endif
};

inline constexpr Pcg64UInt128::Pcg64UInt128()
	: Pcg64UInt128(0, 0)
{}

inline constexpr Pcg64UInt128::Pcg64UInt128(const uint64 high64, const uint64 low64)
#if PH_MATH_PCG64_EMULATED_UINT128
	: m_high64(high64)
	, m_low64(low64)
#else
	: m_128((__uint128_t(high64) << 64) + low64)
#endif
{}

inline uint64 Pcg64UInt128::getHigh64() const
{
#if PH_MATH_PCG64_EMULATED_UINT128
	return m_high64;
#else
	return static_cast<uint64>(m_128 >> 64);
#endif
}

inline uint64 Pcg64UInt128::getLow64() const
{
#if PH_MATH_PCG64_EMULATED_UINT128
	return m_low64;
#else
	return static_cast<uint64>(m_128);
#endif
}

inline Pcg64UInt128 Pcg64UInt128::operator + (const Pcg64UInt128& rhs) const
{
	Pcg64UInt128 result;
#if PH_MATH_PCG64_EMULATED_UINT128
	result.m_low64 = m_low64 + rhs.m_low64;
	result.m_high64 = m_high64 + rhs.m_high64 + (result.m_low64 < m_low64);
#else
	result.m_128 = m_128 + rhs.m_128;
#endif
	return result;
}

inline Pcg64UInt128 Pcg64UInt128::operator * (const Pcg64UInt128& rhs) const
{
	Pcg64UInt128 result;
#if PH_MATH_PCG64_EMULATED_UINT128
	// Multiply the lower parts first
	math::uint64_mul(m_low64, rhs.m_low64, result.m_high64, result.m_low64);

	// The rest work like `uint64_mul()` (see the doc for its impl.), except that the high bit part
	// is discarded (it will overflow)
	result.m_high64 += (m_high64 * rhs.m_low64) + (m_low64 * rhs.m_high64);
#else
	result.m_128 = m_128 * rhs.m_128;
#endif
	return result;
}

}// end namespace detail

/*
 * The following PCG implementation is a version adapted for use in Photon. Most algorithmic parts
 * are left as-is the original reference implementation by Melissa O'Neill and other sources (listed
 * in the documentation of `Pcg64DXSM` class).
 * 
 * The PCG Random Number Generator was developed by Melissa O'Neill <oneill@pcg-random.org>.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For additional information about the PCG random number generation scheme,
 * including its license and other licensing options, visit
 *
 *       http://www.pcg-random.org
 */

/*! @brief PCG-64 DXSM generator.
This is the `pcg_engines::cm_setseq_dxsm_128_64` generator in O'Neill's original `pcg-cpp` implementation.
References:
[1] https://dotat.at/@/2023-06-21-pcg64-dxsm.html
[2] NumPy source code (numpy/numpy/random/src/pcg64/*), version: 308b348bb595bc1b86f9e4bc08be2e3ba5d4f18f
*/
class Pcg64DXSM final : public TUniformRandomBitGenerator<Pcg64DXSM, uint64>
{
public:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(Pcg64DXSM);

	Pcg64DXSM(uint64 initialSequenceHigh64, uint64 initialSequenceLow64);

	/*! @brief Seed the RNG. Specified in two 128-bit parts.
	Initial sequence is equivalent to choosing from one of 2^127 different random number sequences
	(streams), while initial state is equivalent to choosing a starting point in a stream.
	@param initialSequenceHigh64 Higher 64-bit part of the initial sequence.
	@param initialSequenceLow64 Lower 64-bit part of the initial sequence.
	@param initialStateHigh64 Higher 64-bit part of the initial state.
	@param initialStateLow64 Lower 64-bit part of the initial state.
	*/
	Pcg64DXSM(
		uint64 initialSequenceHigh64, uint64 initialSequenceLow64,
		uint64 initialStateHigh64, uint64 initialStateLow64);

	uint64 impl_generate();
	void impl_jumpAhead(uint64 distance);

private:
	using UInt128 = detail::Pcg64UInt128;

	uint64 generateUInt64();

	inline static constexpr auto DEFAULT_STATE = UInt128(0x979C9A98D8462005ull, 0x7D3E9CB6CFE0549Bull);
	inline static constexpr auto DEFAULT_STREAM_ID = UInt128(0x5851F42D4C957F2Dull, 0x14057B7EF767814Full);
	
	// Cheap (half-width) multiplier
	inline static constexpr auto CHEAP_MULTIPLIER_64 = 0xDA942042E4DD58B5ull;

	UInt128 m_state = DEFAULT_STATE;

	/*! Controls which random number sequence (stream) is selected. Must always be odd. */
	UInt128 m_increment = DEFAULT_STREAM_ID;
};

inline Pcg64DXSM::Pcg64DXSM(const uint64 initialSequenceHigh64, const uint64 initialSequenceLow64)
	: Pcg64DXSM(
		initialSequenceHigh64, initialSequenceLow64,
		moremur_bit_mix_64(initialSequenceHigh64), moremur_bit_mix_64(initialSequenceLow64))
{}

inline Pcg64DXSM::Pcg64DXSM(
	const uint64 initialSequenceHigh64, const uint64 initialSequenceLow64,
	const uint64 initialStateHigh64, const uint64 initialStateLow64)
	: Pcg64DXSM()
{
	m_state = UInt128(0ull, 0ull);

	// Ensure `m_increment` is odd (basically doing `(initialSequence << 1u) | 1u`)
	uint64 incrementHigh64 = initialSequenceHigh64 << 1u;
	incrementHigh64 |= initialSequenceLow64 >> 63u;
	uint64 incrementLow64 = (initialSequenceLow64 << 1u) | 1u;
	m_increment = UInt128(incrementHigh64, incrementLow64);

	generateUInt64();
	m_state = m_state + UInt128(initialStateHigh64, initialStateLow64);
	generateUInt64();
}

inline uint64 Pcg64DXSM::impl_generate()
{
	return generateUInt64();
}

inline void Pcg64DXSM::impl_jumpAhead(const uint64 distance)
{
	/* Multi-step advance functions (jump-ahead, jump-back)

	The method used here is based on Brown, "Random Number Generation with Arbitrary Stride,",
	Transactions of the American Nuclear Society (Nov. 1994). The algorithm is very similar to fast
	exponentiation.

	Even though delta is an unsigned integer, we can pass a signed integer to go backwards,
	it just goes "the long way round".
	*/

	constexpr auto ZERO = UInt128(0, 0);
	constexpr auto ONE = UInt128(0, 1);

	auto curMult = UInt128(0, CHEAP_MULTIPLIER_64);
	auto curPlus = m_increment;
	auto accMult = ONE;
	auto accPlus = ZERO;
	uint64 delta = distance;
	while(delta > 0)
	{
		if(delta & 1)
		{
			accMult = accMult * curMult;
			accPlus = accPlus * curMult + curPlus;
		}
		curPlus = (curMult + ONE) * curPlus;
		curMult = curMult * curMult;
		delta /= 2;
	}
	m_state = accMult * m_state + accPlus;
}

inline uint64 Pcg64DXSM::generateUInt64()
{
	// Linear congruential generator
	const UInt128 oldState = m_state;
	m_state = oldState * UInt128(0, CHEAP_MULTIPLIER_64) + m_increment;

	// DXSM (double xor shift multiply) permuted output
	uint64 hi = oldState.getHigh64();
	uint64 lo = oldState.getLow64();
	lo |= 1;
	hi ^= hi >> 32;
	hi *= CHEAP_MULTIPLIER_64;
	hi ^= hi >> 48;
	hi *= lo;

	return hi;
}

}// end namespace ph::math
