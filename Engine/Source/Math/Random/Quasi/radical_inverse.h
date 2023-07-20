#pragma once

#include "Common/assertion.h"
#include "Math/math.h"

#include <concepts>
#include <type_traits>
#include <algorithm>
#include <limits>

namespace ph::math
{

namespace radical_inverse_detail
{

/*! @brief The max value `x` such that `x * base + nextDigit` will not overflow its type.
*/
template<std::integral Value, std::integral Base>
inline constexpr Value reverse_limit(const Value nextDigit, const Base base)
{
	return (std::numeric_limits<Value>::max() - nextDigit) / base;
}

/*! @brief Number of reversed digits that will have effect on the type `Result`.
Due to the limited precision of floating-point types, only a limited number of digits may have an
effect on the final radical inversed value.
*/
template<std::floating_point Result, std::integral Base>
inline constexpr auto num_meaningful_digits(const Base base)
{
	const Result rcpBase = Result(1) / base;

	Base numDigits = 0;
	Result valueScaler = 1;
	while(1 - (base - 1) * valueScaler < 1)
	{
		++numDigits;
		valueScaler *= rcpBase;
	}
	return numDigits;
}

}// end namespace radical_inverse_detail

/*! @brief Compute radical inverse of a value.
@tparam BASE The base to use.
@return Radical inverse of @p value in decimal.
*/
template<auto BASE, std::floating_point Result, std::integral Value>
inline Result radical_inverse(const Value value)
{
	static_assert(std::integral<decltype(BASE)>,
		"`BASE` must be an integer.");
	static_assert(BASE >= 2);

	if constexpr(BASE == 2)
	{
		return math::reverse_bits(value) * Result(0x1p-64);
	}
	else
	{
		constexpr Result rcpBase = Result(1) / BASE;

		// Being safe as we assume a max possible next digit `BASE - 1`, could potentially do better
		// by checking overflow in the loop (just want to be faster here).
		constexpr Value maxSafeValue = radical_inverse_detail::reverse_limit(BASE - 1, BASE);

		// Extract digits from `value` and reassemble them reversely as `unscaledReversedValue`
		Value currentValue = value;
		Value unscaledReversedValue = 0;
		Result scaler = 1;
		while(currentValue > 0 && unscaledReversedValue <= maxSafeValue)
		{
			const Value quotient = currentValue / BASE;
			const Value remainder = currentValue - quotient * BASE;

			unscaledReversedValue = unscaledReversedValue * BASE + remainder;
			scaler *= rcpBase;

			currentValue = quotient;
		}

		return std::clamp<Result>(unscaledReversedValue * scaler, 0, 1);
	}
}

/*! @brief Same as `radical_inverse()`, with permutation ability.
It is guaranteed that no more than `radical_inverse_detail::num_meaningful_digits()` digits will
be processed during the radical inverse.
@param permuter Must be callable with `<BASE>(digit)` and return a permuted digit.
*/
template<auto BASE, std::floating_point Result, std::integral Value, typename DigitPermuter>
inline Result radical_inverse_permuted(const Value value, DigitPermuter permuter)
{
	static_assert(std::integral<decltype(BASE)>,
		"`BASE` must be an integer.");
	static_assert(BASE >= 2);
	static_assert(requires (DigitPermuter p)
		{
			{ p.template operator()<BASE>(Value{})} -> std::same_as<Value>;
		},
		"`DigitPermuter` must be callable with `<BASE>(digit)` and return a permuted digit.");

	constexpr Result rcpBase = Result(1) / BASE;
	constexpr Value maxDigits = radical_inverse_detail::num_meaningful_digits<Result>(BASE);

	// Being safe as we assume a max possible next digit `BASE - 1`, could potentially do better
	// by checking overflow in the loop (just want to be faster here).
	constexpr Value maxSafeValue = radical_inverse_detail::reverse_limit(BASE - 1, BASE);

	// Extract digits from `value` and reassemble them reversely as `unscaledReversedValue`
	// Cannot use the condition `currentValue > 0` to terminate the loop, since `permuter` may
	// produce a non-zero digit from zero.
	Value currentValue = value;
	Value unscaledReversedValue = 0;
	Value numReversedDigits = 0;
	Result scaler = 1;
	while(numReversedDigits < maxDigits && unscaledReversedValue <= maxSafeValue)
	{
		const Value quotient = currentValue / BASE;
		const Value remainder = currentValue - quotient * BASE;
		const Value permutedRemainder = permuter.template operator()<BASE>(remainder);

		unscaledReversedValue = unscaledReversedValue * BASE + permutedRemainder;
		scaler *= rcpBase;
		++numReversedDigits;

		currentValue = quotient;
	}

	return std::clamp<Result>(unscaledReversedValue * scaler, 0, 1);
}

}// end namespace ph::math
