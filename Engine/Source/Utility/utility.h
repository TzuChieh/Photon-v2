#pragma once

#include <utility>
#include <type_traits>
#include <cstring>

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

}// end namespace ph
