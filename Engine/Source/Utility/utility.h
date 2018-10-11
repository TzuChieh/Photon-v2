#pragma once

#include <utility>
#include <type_traits>

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

}// end namespace ph