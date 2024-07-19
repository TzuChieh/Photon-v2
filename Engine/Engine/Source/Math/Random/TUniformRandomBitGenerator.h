#pragma once

#include "Utility/utility.h"
#include "Math/Random/sample.h"

#include <Common/primitive_type.h>

#include <concepts>
#include <bit>
#include <climits>
#include <type_traits>
#include <utility>

namespace ph::math
{
	
template<typename T>
concept CURBG = requires (T instance)
{
	typename T::BitsType;
	instance.generate();
	instance.template generate<uint64>();
	instance.generateSample();
	instance.template generateSample<float64, uint64>();
	{ instance.jumpAhead(uint64{}) } -> std::same_as<void>;
};

template<typename Derived, typename Bits>
class TUniformRandomBitGenerator
{
	// Safer for bit operations (custom types are explicitly allowed).
	static_assert(std::is_unsigned_v<Bits> || std::is_class_v<Bits>);

public:
	using BitsType = Bits;

	Bits generate();

	template<typename TargetBits>
	TargetBits generate();

	template<typename TargetSample = real, typename SourceBits = Bits>
	TargetSample generateSample();

	/*!
	@param distance Number of values of `Bits` to skip.
	*/
	void jumpAhead(uint64 distance);

// Hide special members as this class is not intended to be used polymorphically.
// It is derived class's choice to expose them (by defining them in public) or not.
protected:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(TUniformRandomBitGenerator);
};

template<typename Derived, typename Bits>
inline Bits TUniformRandomBitGenerator<Derived, Bits>::generate()
{
	static_assert(requires (Derived derived)
		{
			{ derived.impl_generate() } -> std::same_as<Bits>;
		},
		"Uniform random bit generator must implement `impl_generate() -> Bits`.");

	return static_cast<Derived&>(*this).impl_generate();
}

template<typename Derived, typename Bits>
template<typename TargetBits>
inline TargetBits TUniformRandomBitGenerator<Derived, Bits>::generate()
{
	static_assert(CHAR_BIT == 8);

	// Safer for bit operations (custom types are explicitly allowed).
	static_assert(std::is_unsigned_v<TargetBits> || std::is_class_v<TargetBits>);

	if constexpr(std::is_unsigned_v<TargetBits>)
	{
		constexpr auto bitsSize = sizeof(Bits);
		constexpr auto targetBitsSize = sizeof(TargetBits);

		if constexpr(targetBitsSize == bitsSize)
		{
			return bitwise_cast<TargetBits>(generate());
		}
		else if constexpr(targetBitsSize == 8 && bitsSize == 4)
		{
			// Generate 8-byte bits by combining two 4-byte bits
			const auto lower4B = bitwise_cast<uint32>(generate());
			const auto higher4B = bitwise_cast<uint32>(generate());
			return bitwise_cast<TargetBits>((static_cast<uint64>(higher4B) << 32) | lower4B);

			// Note: combining 2 `uint32` like this may not yield as good result as using 2 separate
			// generators (with different state and sequence).
		}
		else if constexpr(targetBitsSize == 4 && bitsSize == 8)
		{
			// Just to make sure the cast will truncate `uint64`
			static_assert(static_cast<uint32>(uint64(0xABCD'ABCD'0000'0000) >> 32) == 0xABCD'ABCD);

			// Generate 4-byte bits by using the higher 4-byte in 8-byte bits
			const auto full8B = bitwise_cast<uint64>(generate());
			return bitwise_cast<TargetBits>(static_cast<uint32>(full8B >> 32));

			// Note: generally higher bits may be of better quality
		}
	}
	else
	{
		PH_STATIC_ASSERT_DEPENDENT_FALSE(Derived,
			"No existing implementation can do `Bits` -> `TargetBits`.");
	}
}

template<typename Derived, typename Bits>
template<typename TargetSample, typename SourceBits>
inline TargetSample TUniformRandomBitGenerator<Derived, Bits>::generateSample()
{
	static_assert(CHAR_BIT == 8);

	// Safer for bit operations (custom types are explicitly allowed).
	static_assert(std::is_unsigned_v<SourceBits> || std::is_class_v<SourceBits>);

	// Samples must be within [0, 1], makes no sense for using integral types
	// (custom types are explicitly allowed).
	static_assert(std::is_floating_point_v<TargetSample> || std::is_class_v<TargetSample>);

	return bits_to_sample<TargetSample>(generate<SourceBits>());
}

template<typename Derived, typename Bits>
inline void TUniformRandomBitGenerator<Derived, Bits>::jumpAhead(const uint64 distance)
{
	static_assert(requires (Derived derived)
		{
			{ derived.impl_jumpAhead(std::declval<uint64>())} -> std::same_as<void>;
		},
		"Uniform random bit generator must implement `impl_jumpAhead(uint64 distance) -> void`.");

	static_cast<Derived&>(*this).impl_jumpAhead(distance);
}

}// end namespace ph::math
