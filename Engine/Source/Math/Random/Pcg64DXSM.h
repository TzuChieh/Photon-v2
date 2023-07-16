#pragma once

#include "Math/Random/TUniformRandomBitGenerator.h"
#include "Common/primitive_type.h"
#include "Math/hash.h"
#include "Math/math.h"

#include <type_traits>

// This is a good reference on built-in 128-bit integer:
// https://stackoverflow.com/questions/16088282/is-there-a-128-bit-integer-in-gcc
// In the future, consider using `_BitInt`

/*! Define as 1 to force the use of emulated 128-bit math. */
#define PH_MATH_PCG64_FORCE_EMULATED_UINT128 0

#define PH_MATH_PCG64_EMULATED_UINT128 (!defined(__SIZEOF_INT128__) || PH_MATH_PCG64_FORCE_EMULATED_UINT128)

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

	Pcg64UInt128 operator + (Pcg64UInt128 rhs) const;
	Pcg64UInt128 operator * (Pcg64UInt128 rhs) const;

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

inline Pcg64UInt128 Pcg64UInt128::operator + (Pcg64UInt128 rhs) const
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

inline Pcg64UInt128 Pcg64UInt128::operator * (Pcg64UInt128 rhs) const
{
	Pcg64UInt128 result;
#if PH_MATH_PCG64_EMULATED_UINT128
	// Multiply the lower parts first
	math::uint64_mul(m_low64, rhs.m_low64, result.m_high64, result.m_low64);

	// The rest work like `uint64_mul()` (see the doc for its impl.), except that the high bit part
	// is discarded (it will overflow)
	result.m_high64 += (m_high64 * rhs.m_low64) + (rhs.m_low64 * m_high64);
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
 * PCG Random Number Generation for C.
 *
 * Copyright 2014 Melissa O'Neill <oneill@pcg-random.org>
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

/*!
References:
[1] https://dotat.at/@/2023-06-21-pcg64-dxsm.html
[2] NumPy source code (numpy/numpy/random/src/pcg64/*), version: 308b348bb595bc1b86f9e4bc08be2e3ba5d4f18f
*/
class Pcg64DXSM final : public TUniformRandomBitGenerator<Pcg64DXSM, uint64>
{
public:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(Pcg64DXSM);

	explicit Pcg32(uint64 initialState);

	/*! @brief Seed the RNG. Specified in two parts.
	@param initialState Equivalent to choosing a starting point in a stream.
	@param initialSequence Equivalent to choosing from one of 2^63 different random number sequences.
	*/
	Pcg32(uint64 initialState, uint64 initialSequence);

	uint32 impl_generate();
	void impl_jumpAhead(uint64 distance);

private:
	uint32 generateUInt32();

	inline static constexpr uint64 DEFAULT_STATE = 0x853C49E6748FEA9BULL;
	inline static constexpr uint64 DEFAULT_STREAM_ID = 0xDA3E39CB94B95BDBULL;
	inline static constexpr uint64 MULTIPLIER = 0x5851F42D4C957F2DULL;

	uint64 m_state = DEFAULT_STATE;

	/*! Controls which random number sequence (stream) is selected. Must always be odd. */
	uint64 m_increment = DEFAULT_STREAM_ID;
};

inline Pcg32::Pcg32(const uint64 initialState)
	: Pcg32(initialState, moremur_bit_mix(initialState))
{}

inline Pcg32::Pcg32(const uint64 initialState, const uint64 streamId)
	: Pcg32()
{
	m_state = 0U;

	// Ensure `m_increment` is odd
	m_increment = (streamId << 1u) | 1u;

	generateUInt32();
	m_state += initialState;
	generateUInt32();
}

inline uint32 Pcg32::impl_generate()
{
	return generateUInt32();
}

inline void Pcg32::impl_jumpAhead(const uint64 distance)
{
	uint64 curMult = MULTIPLIER;
	uint64 curPlus = m_increment;
	uint64 accMult = 1u;
	uint64 accPlus = 0u;
	uint64 delta = distance;
	while(delta > 0)
	{
		if(delta & 1)
		{
			accMult *= curMult;
			accPlus = accPlus * curMult + curPlus;
		}
		curPlus = (curMult + 1) * curPlus;
		curMult *= curMult;
		delta /= 2;
	}
	m_state = accMult * m_state + accPlus;
}

inline uint32 Pcg32::generateUInt32()
{
	uint64 oldstate = m_state;
	m_state = oldstate * MULTIPLIER + m_increment;
	uint32 xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
	uint32 rot = oldstate >> 59u;
	return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

}// end namespace ph::math
