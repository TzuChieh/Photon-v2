#pragma once

#include "Math/General/TVectorNBase.h"
#include "Math/math.h"

#include <Common/assertion.h>

#include <cmath>

namespace ph::math
{

template<typename Derived, typename T, std::size_t N>
inline T TVectorNBase<Derived, T, N>::dot(const Derived& rhs) const
{
	return ::ph::math::dot_product(m, static_cast<const Self&>(rhs).m);
}

template<typename Derived, typename T, std::size_t N>
inline T TVectorNBase<Derived, T, N>::dot(const T rhs) const
{
	return sum() * rhs;
}

template<typename Derived, typename T, std::size_t N>
inline T TVectorNBase<Derived, T, N>::absDot(const Derived& rhs) const
{
	return std::abs(dot(rhs));
}

template<typename Derived, typename T, std::size_t N>
inline T TVectorNBase<Derived, T, N>::absDot(const T rhs) const
{
	return std::abs(dot(rhs));
}

template<typename Derived, typename T, std::size_t N>
inline T TVectorNBase<Derived, T, N>::length() const
{
	return ::ph::math::length(m);
}

template<typename Derived, typename T, std::size_t N>
inline T TVectorNBase<Derived, T, N>::lengthSquared() const
{
	return ::ph::math::length_squared(m);
}

template<typename Derived, typename T, std::size_t N>
inline auto TVectorNBase<Derived, T, N>::normalize() const
-> Derived
{
	return Derived(static_cast<const Derived&>(*this)).normalizeLocal();
}

template<typename Derived, typename T, std::size_t N>
inline auto TVectorNBase<Derived, T, N>::normalizeLocal()
-> Derived&
{
	::ph::math::normalize(m);

	return static_cast<Derived&>(*this);
}

template<typename Derived, typename T, std::size_t N>
inline auto TVectorNBase<Derived, T, N>::safeNormalize(const Derived& fallback) const
-> Derived
{
	const Derived normalized = normalize();
	return !normalized.isZero() && normalized.isFinite() ? normalized : fallback;
}

template<typename Derived, typename T, std::size_t N>
inline std::size_t TVectorNBase<Derived, T, N>::minDimension() const
{
	// minIndex() is not exposed; use "this" to access it in current scope
	return this->minIndex();
}

template<typename Derived, typename T, std::size_t N>
inline std::size_t TVectorNBase<Derived, T, N>::maxDimension() const
{
	// maxIndex() is not exposed; use "this" to access it in current scope
	return this->maxIndex();
}

#if PH_COMPILER_HAS_P2468R2

template<typename Derived, typename T, std::size_t N>
inline bool TVectorNBase<Derived, T, N>::operator == (const Self& other) const
{
	return Base::operator == (other);
}

#endif

}// end namespace ph::math
