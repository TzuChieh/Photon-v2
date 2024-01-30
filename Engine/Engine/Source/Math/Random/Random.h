#pragma once

#include <Common/primitive_type.h>

#include <cstddef>
#include <array>

namespace ph::math
{

/*! @brief Get random values from default RNG.
*/
class Random final
{
public:
	/*! @brief Get a uniform random value in [0, 1].
	@note Thread-safe. Each thread gets a different sequence.
	*/
	static real sample();

	/*! @brief Similar to `sample()`, except the sample is N-dimensional.
	*/
	template<std::size_t N, typename T = real>
	static std::array<T, N> sampleND();

	/*! @brief Get a uniform random integer value in [`lowerBound`, `upperBound`).
	@note Thread-safe. Each thread gets a different sequence.
	*/
	static std::size_t index(std::size_t lowerBound, std::size_t upperBound);

	/*!
	@return 32 uniform random bits.
	*/
	static uint32 bits32();

	/*!
	@return 64 uniform random bits.
	*/
	static uint64 bits64();
};

}// end namespace ph::math

#include "Math/Random/Random.ipp"
