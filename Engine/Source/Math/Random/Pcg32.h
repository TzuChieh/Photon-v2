#pragma once

#include "Math/Random/TUniformRandomBitGenerator.h"
#include "Common/primitive_type.h"
#include "Math/hash.h"

#include <type_traits>

namespace ph::math
{

/*
 * The following PCG implementation is a version adapted for use in Photon. Most algorithmic parts
 * are left as-is the original reference implementation by Melissa O'Neill and other sources (listed
 * in the documentation of `Pcg32` class).
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
[2] https://github.com/imneme/pcg-c-basic
[3] https://github.com/wjakob/pcg32
*/
class Pcg32 final : public TUniformRandomBitGenerator<Pcg32, uint32>
{
public:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(Pcg32);

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
