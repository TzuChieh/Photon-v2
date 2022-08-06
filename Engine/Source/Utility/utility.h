#pragma once

#include <utility>
#include <type_traits>
#include <cstring>
#include <string>

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

template<typename EnumType>
inline std::string enum_to_string(EnumType value)
{
	static_assert(std::is_enum_v<EnumType>, 
		"EnumType accepts only enum (both scoped and unscoped).");

	using ValueType = std::underlying_type_t<EnumType>;
	return std::to_string(static_cast<ValueType>(value));
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

}// end namespace ph
