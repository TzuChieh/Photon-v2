#pragma once

#include <utility>
#include <type_traits>
#include <cstring>

namespace ph
{

// TODO: make rvalue input possible? (beware of dangling reference after return!)

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
		return t;
	}
	else
	{
		// Dereferencing is a lvalue expression, the return type should deduce
		// to lvalue reference (possibly cv-qualified).
		return *t;
	}
}

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

}// end namespace ph