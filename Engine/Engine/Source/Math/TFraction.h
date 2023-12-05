#pragma once

#include "Math/math_fwd.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <utility>
#include <type_traits>

namespace ph::math
{

template<typename T>
class TFraction final
{
public:
	constexpr inline TFraction() = default;
	constexpr TFraction(T nume, T deno);

	// TODO: arithemtics

	template<typename U = real>
	constexpr U toReal() const;

	template<typename U>
	constexpr explicit operator U () const;

private:
	T m_nume;
	T m_deno;
};

// In-header Implementations:

template<typename T>
inline constexpr TFraction<T>::TFraction(T nume, T deno) :
	m_nume(std::move(nume)),
	m_deno(std::move(deno))
{}

template<typename T>
template<typename U>
inline constexpr U TFraction<T>::toReal() const
{
	static_assert(std::is_floating_point_v<U>);

	return m_nume / m_deno;
}

template<typename T>
template<typename U>
inline constexpr TFraction<T>::operator U () const
{
	return toReal<U>();
}

}// end namespace ph::math
