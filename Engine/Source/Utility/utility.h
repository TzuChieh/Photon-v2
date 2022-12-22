#pragma once

#include "Utility/exception.h"
#include "Common/assertion.h"
#include "Utility/traits.h"

#include <utility>
#include <type_traits>
#include <cstring>
#include <string>
#include <climits>
#include <limits>
#include <concepts>

namespace ph
{

// TODO: make rvalue input possible? (beware of dangling reference after return!)
// TODO: consider using overloads: https://stackoverflow.com/questions/14466620/c-template-specialization-calling-methods-on-types-that-could-be-pointers-or

template<typename T>
inline decltype(auto) pointer_access(T& t)
{
	if constexpr(std::is_pointer_v<T>)
	{
		return t;
	}
	else
	{
		return &(t);
	}
}

template<typename T>
inline decltype(auto) regular_access(T& t)
{
	if constexpr(!std::is_pointer_v<T>)
	{
		// TODO: surround by parentheses to make it an expression for returning lvalue reference?
		return t;
	}
	else
	{
		// Dereferencing is a lvalue expression, the return type should deduce
		// to lvalue reference (possibly cv-qualified).
		return *t;
	}
}

// TODO: replace this with C++20 std::bit_cast
template<typename Source, typename Target>
inline Target bitwise_cast(const Source source)
{
	static_assert(std::is_arithmetic_v<Source> && std::is_arithmetic_v<Target>);

	static_assert(sizeof(Source) == sizeof(Target),
		"Source and Target should have same size");

	Target target;
	std::memcpy(&target, &source, sizeof(Source));
	return target;
}

// TODO: replace this with C++20 std::endian
inline bool is_big_endian()
{
	static_assert(sizeof(int) > sizeof(char));

	const int i = 0x07;
	return reinterpret_cast<const char*>(&i)[0] != '\x07';
}

template<typename T>
inline consteval std::size_t sizeof_in_bits()
{
	return CHAR_BIT * sizeof(T);
}

template<CIsEnum EnumType>
inline constexpr auto enum_to_value(const EnumType enumValue)
{
	using ValueType = std::underlying_type_t<EnumType>;
	return static_cast<ValueType>(enumValue);
}

template<CIsEnum EnumType>
inline std::string enum_to_string(const EnumType enumValue)
{
	return std::to_string(enum_to_value(enumValue));
}

template<CIsEnumWithSizeInfo EnumType>
inline constexpr auto enum_size()
{
	return enum_to_value(EnumType::SIZE);
}

template<typename T, T VALUE>
struct TNonTypeTemplateArgDummy final
{};

/*! @brief Cast `const` value to non-`const` value.
When code duplication is undesirable between `const` and non-`const` getters, one can reuse the `const`
version in the following way (providing the caller object is actually non-`const`):

```C++
const T& f() const
{
	return something_complicated();
}

decltype(auto) f()
{
	return mutable_cast(std::as_const(*this).f());
}
```

The following overloads ensures that `volatile` qualifier will not be accidently cast away unlike common
`const_cast` approaches. Moreover, deleted overload is there to guard against the case where `T` is
returned by `f()` and generating a dangling reference (when returning value, we do not need a
non-`const` overload anyway).

Reference: https://stackoverflow.com/a/47369227

@note Reusing non-`const` getter by casting it to `const` in `const` getter may produce UB. These 
overloads prevent most misuse cases, still one should be careful using them in the implementation.
*/
///@{
template<typename T>
inline constexpr T& mutable_cast(const T& value) noexcept
{
    return const_cast<T&>(value);
}

template<typename T>
inline constexpr T* mutable_cast(const T* value) noexcept
{
    return const_cast<T*>(value);
}

template<typename T>
inline constexpr T* mutable_cast(T* value) noexcept
{
    return value;
}

template<typename T>
inline void mutable_cast(const T&&) = delete;
///@}

template<std::integral DstType, std::integral SrcType>
inline DstType lossless_integer_cast(const SrcType src)
{
	using SrcLimits = std::numeric_limits<SrcType>;
	using DstLimits = std::numeric_limits<DstType>;

	// Note that the use of `std::cmp_<X>` functions are important as the comparisons 
	// may be signed <-> unsigned comparisons, which may cause signed limits to overflow

	// TODO: we may need to cast src to some integer first to support char and bool types (they are not supported by cmp functions)

	constexpr bool mayHavePositiveOverflow = std::cmp_greater(SrcLimits::max(), DstLimits::max());
	constexpr bool mayHaveNegativeOverflow = std::cmp_less(SrcLimits::lowest(), DstLimits::lowest());

	if constexpr(mayHavePositiveOverflow)
	{
		if(std::cmp_greater(src, DstLimits::max()))
		{
			throw_formatted<OverflowException>("cast results in positive overflow: {} exceeds the limit {}",
				src, DstLimits::max());
		}
	}

	if constexpr(mayHaveNegativeOverflow)
	{
		if(std::cmp_less(src, DstLimits::lowest()))
		{
			throw_formatted<OverflowException>("cast results in negative overflow: {} exceeds the limit {}",
				src, DstLimits::lowest());
		}
	}

	// All possible integer overflow scenarios are checked so it is safe to cast now
	return static_cast<DstType>(src);
}

template<std::floating_point DstType, std::floating_point SrcType>
inline DstType lossless_float_cast(const SrcType src)
{
	// Nothing to do if both types are the same
	if constexpr(std::is_same_v<SrcType, DstType>)
	{
		return src;
	}
	// If we are converting to a wider floating-point type, generally it will be lossless
	else if constexpr(sizeof(DstType) > sizeof(SrcType))
	{
		// We need both types to be IEEE-754
		static_assert(std::numeric_limits<SrcType>::is_iec559);
		static_assert(std::numeric_limits<DstType>::is_iec559);

		return static_cast<DstType>(src);
	}
	// Otherwise, cast to `DstType` then back to `SrcType` and see if there is any difference
	else
	{
		const auto dst = static_cast<DstType>(src);
		const auto dstBackToSrc = static_cast<SrcType>(dst);
		if(src != dstBackToSrc)
		{
			throw_formatted<NumericException>("cast results in numeric precision loss: {} -> {}",
				src, dstBackToSrc);
		}

		return dst;
	}
}

/*! @brief Cast numeric value to another type without any loss of information.
If there is any possible overflow or numeric precision loss, exception is thrown.
@exception OverflowException If overflow happens.
@exception Numericxception If any numeric precision loss happens.
*/
template<CIsNumber DstType, CIsNumber SrcType>
inline DstType lossless_cast(const SrcType src)
{
	// Integer -> Integer
	if constexpr(std::is_integral_v<SrcType> && std::is_integral_v<DstType>)
	{
		return lossless_integer_cast<DstType>(src);
	}
	// Integer -> Floating-point
	else if constexpr(std::is_integral_v<SrcType> && std::is_floating_point_v<DstType>)
	{
		// TODO
		PH_ASSERT_UNREACHABLE_SECTION();
		return 0;
	}
	// Floating-point -> Integer
	else if constexpr(std::is_floating_point_v<SrcType> && std::is_integral_v<DstType>)
	{
		// TODO
		PH_ASSERT_UNREACHABLE_SECTION();
		return 0;
	}
	// Floating-point -> Floating-point
	else
	{
		static_assert(std::is_floating_point_v<SrcType> && std::is_floating_point_v<DstType>);

		return lossless_float_cast<DstType>(src);
	}
}

template<CIsNumber DstType, CIsNumber SrcType>
inline DstType lossless_cast(const SrcType src, DstType* const out_dst)
{
	PH_ASSERT(out_dst);

	*out_dst = lossless_cast<DstType>(src);
	return *out_dst;
}

}// end namespace ph
