#pragma once

#include "Utility/traits.h"

#include <Common/assertion.h>
#include <Common/exceptions.h>

#include <bit>
#include <concepts>
#include <cstddef>
#include <cstring>
#include <limits>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <version>

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

/*! @brief Helper to define defaulted rule of 5 special class members (inlined).
*/
#define PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(ClassType)\
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS_NO_DTOR(ClassType);\
	inline ~ClassType() = default

namespace ph
{

/*! @brief Access a target's member consistently using `.` or `->` operators.
Note that these functions treat smart pointers as regular objects (`ptr_access()` and `ref_access()`
will refer to the object itself, not its managed data).
A nice discussion for the topic: https://stackoverflow.com/questions/14466620/c-template-specialization-calling-methods-on-types-that-could-be-pointers-or.
*/
///@{
template<typename T>
inline T* ptr_access(T* const ptr)
{
	return ptr;
}

template<typename T>
inline T* ptr_access(T& ref)
{
	return &ref;
}

/*!
All remaining types are forbidden. This forwarding reference overload also prevent the use
of rvalue to guard against potential dangling pointer.
*/
template<typename T>
inline T* ptr_access(T&& ref) = delete;

template<typename T>
inline T& ref_access(T& ref)
{
	return ref;
}

template<typename T>
inline T& ref_access(T* const ptr)
{
	PH_ASSERT(ptr);
	return *ptr;
}

/*!
Cannot dereference a `nullptr`.
*/
inline void ref_access(std::nullptr_t /* ptr */) = delete;

/*!
All remaining types are forbidden. This forwarding reference overload also prevent the use
of rvalue to guard against potential dangling reference.
*/
template<typename T>
inline T& ref_access(T&& ref) = delete;
///@}

/*!
Effectively the same as `std::bit_cast()`. This function also works without C++20.
*/
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

/*!
Effectively testing `std::endian::native == std::endian::big`. This function also works without C++20.
*/
inline consteval bool is_big_endian()
{
#if __cpp_lib_endian
	return std::endian::native == std::endian::big;
#else
	static_assert(sizeof(int) > sizeof(char));

	constexpr int i = 0x07;
	return reinterpret_cast<const char*>(&i)[0] != '\x07';
#endif
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
	// ... complex logics ...
	return ...;
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

}// end namespace ph
