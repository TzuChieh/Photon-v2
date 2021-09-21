#pragma once

#include "Math/General/TArithmeticArrayBase.h"
#include "Math/math_fwd.h"

#include <cstddef>
#include <type_traits>

namespace ph::math
{

template<typename Derived, typename T, std::size_t N>
class TVectorNBase : public TArithmeticArrayBase<Derived, T, N>
{
private:
	using Base = TArithmeticArrayBase<Derived, T, N>;

protected:
	using Base::m;

// Hide special members as this class is not intended to be used polymorphically.
// It is derived class's choice to expose them (by defining them in public) or not.
protected:
	inline TVectorNBase() = default;
	inline TVectorNBase(const TVectorNBase& other) = default;
	inline TVectorNBase(TVectorNBase&& other) = default;
	inline TVectorNBase& operator = (const TVectorNBase& rhs) = default;
	inline TVectorNBase& operator = (TVectorNBase && rhs) = default;
	inline ~TVectorNBase() = default;

public:
	using Base::Base;

	template<typename U>
	explicit TVectorNBase(const std::array<U, N>& values);

	template<typename U>
	explicit TVectorNBase(const U* values);

	template<typename U>
	explicit TVectorNBase(const std::vector<U>& values);

	T dot(const Derived& rhs) const;
	T dot(T rhs) const;
	T absDot(const Derived& rhs) const;
	T absDot(T rhs) const;

	T length() const;
	T lengthSquared() const;

	// Notice that normalizing a integer typed vector will result in 0-vector 
	// most of the time.
	Derived normalize() const;
	Derived& normalizeLocal();

	template<typename = std::enable_if_t<std::is_signed_v<T>>>
	Derived negate() const;

	template<typename = std::enable_if_t<std::is_signed_v<T>>>
	Derived& negateLocal();

	std::size_t minDimension() const;
	std::size_t maxDimension() const;

	using Base::NUM_ELEMENTS;
	using Base::size;

	using Base::add;
	using Base::addLocal;

	using Base::sub;
	using Base::subLocal;

	using Base::mul;
	using Base::mulLocal;

	using Base::div;
	using Base::divLocal;

	using Base::pow;
	using Base::powLocal;

	using Base::sqrt;
	using Base::sqrtLocal;

	using Base::clamp;
	using Base::clampLocal;

	using Base::abs;
	using Base::absLocal;

	using Base::rcp;
	using Base::rcpLocal;

	using Base::complement;
	using Base::complementLocal;

	using Base::sum;
	using Base::avg;
	using Base::min;
	using Base::max;
	using Base::ceil;
	using Base::floor;
	using Base::product;
	using Base::lerp;
	using Base::isZero;
	using Base::isNonNegative;
	using Base::isFinite;
	using Base::set;
	using Base::begin;
	using Base::end;
	using Base::isEqual;
	using Base::isNear;
	using Base::toString;
	using Base::toVector;

	using Base::operator [];
	using Base::operator ==;
	using Base::operator !=;

	using Base::operator +;
	using Base::operator +=;
	using Base::operator -;
	using Base::operator -=;
	using Base::operator *;
	using Base::operator *=;
	using Base::operator /;
	using Base::operator /=;
};

}// end namespace ph::math

#include "Math/General/TVectorNBase.ipp"
