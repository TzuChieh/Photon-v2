#pragma once

#include "Utility/exception.h"
#include "Common/assertion.h"
#include "Utility/traits.h"

#include <utility>
#include <type_traits>
#include <cstring>
#include <string>
#include <string_view>
#include <climits>
#include <limits>
#include <concepts>
#include <bit>
#include <version>
#include <variant>

/*! @brief Helper to declare rule of 5 special class members.
Destructor is not included.
*/
#define PH_DECLARE_RULE_OF_5_MEMBERS_NO_DTOR(ClassType)\
	ClassType();\
	ClassType(const ClassType& other);\
	ClassType(ClassType&& other) noexcept;\
	ClassType& operator = (const ClassType& rhs);\
	ClassType& operator = (ClassType&& rhs) noexcept

/*! @brief Helper to define defaulted rule of 5 special class members.
Destructor is not included.
*/
#define PH_DEFINE_RULE_OF_5_MEMBERS_NO_DTOR(ClassType)\
	ClassType::ClassType() = default;\
	ClassType::ClassType(const ClassType& other) = default;\
	ClassType::ClassType(ClassType&& other) noexcept = default;\
	ClassType& ClassType::operator = (const ClassType& rhs) = default;\
	ClassType& ClassType::operator = (ClassType&& rhs) noexcept = default

/*! @brief Helper to define defaulted rule of 5 special class members (inlined).
Destructor is not included.
*/
#define PH_DEFINE_INLINE_RULE_OF_5_MEMBERS_NO_DTOR(ClassType)\
	inline ClassType() = default;\
	inline ClassType(const ClassType& other) = default;\
	inline ClassType(ClassType&& other) noexcept = default;\
	inline ClassType& operator = (const ClassType& rhs) = default;\
	inline ClassType& operator = (ClassType&& rhs) noexcept = default

/*! @brief Helper to declare rule of 5 special class members.
*/
#define PH_DECLARE_RULE_OF_5_MEMBERS(ClassType)\
	PH_DECLARE_RULE_OF_5_MEMBERS_NO_DTOR(ClassType);\
	~ClassType()

/*! @brief Helper to define defaulted rule of 5 special class members.
*/
#define PH_DEFINE_RULE_OF_5_MEMBERS(ClassType)\
	PH_DEFINE_RULE_OF_5_MEMBERS_NO_DTOR(ClassType);\
	ClassType::~ClassType() = default

/*! @brief Helper to define defaultedrule of 5 special class members (inlined).
*/
#define PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(ClassType)\
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS_NO_DTOR(ClassType);\
	inline ~ClassType() = default

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

template<typename Target, typename Source>
inline Target bitwise_cast(const Source& source)
{
	static_assert(std::is_trivially_copyable_v<Source>);
	static_assert(std::is_trivially_copyable_v<Target>);

	static_assert(sizeof(Source) == sizeof(Target),
		"Source and Target must have the same size");

	if constexpr(std::is_same_v<Source, Target>)
	{
		return source;
	}
	else
	{
#if __cpp_lib_bit_cast
		return std::bit_cast<Target>(source);
#else
		Target target;
		std::memcpy(&target, &source, sizeof(Source));
		return target;
#endif
	}
}

inline bool is_big_endian()
{
#if __cpp_lib_endian
	return std::endian::native == std::endian::big;
#else
	static_assert(sizeof(int) > sizeof(char));

	const int i = 0x07;
	return reinterpret_cast<const char*>(&i)[0] != '\x07';
#endif
}

template<typename T>
inline consteval std::size_t sizeof_in_bits()
{
	return CHAR_BIT * sizeof(T);
}

template<CEnum EnumType>
inline constexpr auto enum_to_value(const EnumType enumValue)
{
	using ValueType = std::underlying_type_t<EnumType>;
	return static_cast<ValueType>(enumValue);
}

template<CEnum EnumType>
inline std::string enum_to_string(const EnumType enumValue)
{
	return std::to_string(enum_to_value(enumValue));
}

template<CEnumWithSizeInfo EnumType>
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

template<typename TypeInVariant, typename VariantType, std::size_t D_INDEX = 0>
inline constexpr std::size_t variant_index_of() noexcept
{
	if constexpr(D_INDEX == std::variant_size_v<VariantType>)
	{
		static_assert(D_INDEX < std::variant_size_v<VariantType>,
			"`TypeInVariant` must be one of the types in variant.");
	}
	else if constexpr(std::is_same_v<std::variant_alternative_t<D_INDEX, VariantType>, TypeInVariant>)
	{
		return D_INDEX;
	}
	else
	{
		return variant_index_of<TypeInVariant, VariantType, D_INDEX + 1>();
	}
}

template<typename TypeInVariant, typename InVariantType>
inline constexpr std::size_t variant_index_of(const InVariantType& /* variant */) noexcept
{
	using VariantType = std::remove_cvref_t<InVariantType>;

	return variant_index_of<TypeInVariant, VariantType>();
}

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
template<CNumber DstType, CNumber SrcType>
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

template<CNumber DstType, CNumber SrcType>
inline DstType lossless_cast(const SrcType src, DstType* const out_dst)
{
	PH_ASSERT(out_dst);

	*out_dst = lossless_cast<DstType>(src);
	return *out_dst;
}

}// end namespace ph
