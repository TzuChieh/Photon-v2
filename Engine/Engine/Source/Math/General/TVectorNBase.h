#pragma once

#include "Math/General/TArithmeticArrayBase.h"
#include "Math/math_fwd.h"
#include "Utility/utility.h"

#include <cstddef>


namespace ph::math
{

template<typename Derived, typename T, std::size_t N>
class TVectorNBase : public TArithmeticArrayBase<Derived, T, N>
{
private:
	using Base = TArithmeticArrayBase<Derived, T, N>;
	using Self = TVectorNBase;

protected:
	using Base::m;

// Hide special members as this class is not intended to be used polymorphically.
// It is derived class's choice to expose them (by defining them in public) or not.
protected:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(TVectorNBase);

public:
	using Base::Base;

	T dot(const Derived& rhs) const;
	T dot(T rhs) const;
	T absDot(const Derived& rhs) const;
	T absDot(T rhs) const;

	T length() const;
	T lengthSquared() const;

	/*! @brief Normalize the vector.
	Notice that normalizing a integer typed vector will result in 0-vector most of the time.
	*/
	///@{
	Derived normalize() const;
	Derived& normalizeLocal();
	///@}

	Derived safeNormalize(const Derived& fallback = Derived{}) const;

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

	using Base::exp;
	using Base::expLocal;

	using Base::sqrt;
	using Base::sqrtLocal;

	using Base::clamp;
	using Base::clampLocal;
	using Base::safeClamp;
	using Base::safeClampLocal;

	using Base::abs;
	using Base::absLocal;

	using Base::rcp;
	using Base::rcpLocal;

	using Base::complement;
	using Base::complementLocal;

	using Base::negate;
	using Base::negateLocal;

	using Base::sum;
	using Base::avg;
	using Base::product;
	using Base::min;
	using Base::minIndex;
	using Base::max;
	using Base::maxIndex;
	using Base::ceil;
	using Base::floor;
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
	using Base::toArray;
	using Base::toSpan;
	using Base::toView;

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

public:
	/*! @brief Non-member operators for expressions beginning with a single element value.
	*/
	///@{
	friend Derived operator + (const T rhs, const Derived& lhs)
	{
		return lhs.add(rhs);
	}

	friend Derived operator - (const T rhs, const Derived& lhs)
	{
		return lhs.negate().add(rhs);
	}

	friend Derived operator * (const T rhs, const Derived& lhs)
	{
		return lhs.mul(rhs);
	}

	friend Derived operator / (const T rhs, const Derived& lhs)
	{
		return lhs.rcp().mul(rhs);
	}
	///@}
};

}// end namespace ph::math

#include "Math/General/TVectorNBase.ipp"
