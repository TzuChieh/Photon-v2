#pragma once

#include "Common/primitive_type.h"
#include "Utility/utility.h"

#include <concepts>
#include <bit>
#include <climits>
#include <type_traits>

namespace ph::math
{
	
template<typename T>
concept CUniformRandomBitGenerator = requires (T instance)
{
	typename T::BitsType;
	instance.generate();
	instance.template generate<uint64>();
	instance.generateSample();
	instance.template generateSample<float64>();
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

	template<typename TargetSample = real>
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

	constexpr auto bitsSize = sizeof(Bits);
	constexpr auto targetBitsSize = sizeof(TargetBits);

	if constexpr(targetBitsSize == bitsSize)
	{
		return bitwise_cast<TargetBits>(generate());
	}
	else if constexpr(targetBitsSize == 8 && bitsSize == 4)
	{
		// Generate 8-byte bits by combining two 4-byte bits
		const auto lower4B = bitwise_cast<uint64>(generate());
		const auto higher4B = bitwise_cast<uint64>(generate()) << 32;
		return bitwise_cast<TargetBits>(higher4B | lower4B);
	}
	else if constexpr(targetBitsSize == 4 && bitsSize == 8)
	{
		// Generate 4-byte bits by using the higher 4-byte in 8-byte bits
		const auto full8B = bitwise_cast<uint64>(generate());
		return bitwise_cast<TargetBits>(static_cast<uint32>(full8B >> 32));
	}
	else
	{
		PH_STATIC_ASSERT_DEPENDENT_FALSE(Derived,
			"No existing implementation can do `Bits` -> `TargetBits`.");
	}
}

template<typename Derived, typename Bits>
template<typename TargetSample>
inline TargetSample TUniformRandomBitGenerator<Derived, Bits>::generateSample()
{
	// Samples must be within [0, 1], makes no sense for using integral types
	// (custom types are explicitly allowed).
	static_assert(std::is_floating_point_v<TargetSample> || std::is_class_v<TargetSample>);

	if constexpr(std::is_same_v<TargetSample, float32>)
	{
		const float32 sample = generate<uint32>() / 0x1p-32f;
		PH_ASSERT_IN_RANGE_EXCLUSIVE(sample, 0.0f, 1.0f);
		return sample;
	}
	else if constexpr(std::is_same_v<TargetSample, float64>)
	{
		const float64 sample = generate<uint64>() / 0x1p-64;
		PH_ASSERT_IN_RANGE_EXCLUSIVE(sample, 0.0, 1.0);
		return sample;
	}
	else
	{
		PH_STATIC_ASSERT_DEPENDENT_FALSE(Derived,
			"No existing implementation can do `Bits` -> `TargetSample`.");
	}
}

template<typename Derived, typename Bits>
inline void TUniformRandomBitGenerator<Derived, Bits>::jumpAhead(const uint64 distance)
{
	static_assert(requires (Derived derived)
		{
			{ derived.impl_jumpAhead(uint64{})} -> std::same_as<void>;
		},
		"Uniform random bit generator must implement `impl_jumpAhead(uint64 distance) -> void`.");

	static_cast<Derived&>(*this).impl_jumpAhead(distance);
}

}// end namespace ph::math
