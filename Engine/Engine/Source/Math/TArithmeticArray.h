#pragma once

#include "Math/General/TArithmeticArrayBase.h"

#include <Common/compiler.h>

#include <cstddef>
#include <array>

namespace ph::math
{

template<typename T, std::size_t N>
class TArithmeticArray final : public TArithmeticArrayBase<TArithmeticArray<T, N>, T, N>
{
private:
	using Self = TArithmeticArray;
	using Base = TArithmeticArrayBase<TArithmeticArray<T, N>, T, N>;

protected:
	using Base::m;

public:
	inline TArithmeticArray() = default;
	inline TArithmeticArray(const TArithmeticArray& other) = default;
	inline TArithmeticArray(TArithmeticArray&& other) = default;
	inline TArithmeticArray& operator = (const TArithmeticArray& rhs) = default;
	inline TArithmeticArray& operator = (TArithmeticArray&& rhs) = default;
	inline ~TArithmeticArray() = default;

	using Base::Base;

	template<typename U>
	explicit TArithmeticArray(const TArithmeticArray<U, N>& other);

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
	using Base::toArray;

	using Base::operator [];

#if !PH_COMPILER_HAS_P2468R2
	using Base::operator ==;
	using Base::operator !=;
#else
	/*! With using-declaration, it is as if the operator is defined in this class. The first argument
	(the implicit `this`) will be `Self` in this case, while `other` will be the base type. This will
	cause ambiguity when the operator is being rewritten, hence we declare the operator manually.
	*/
	bool operator == (const Self& other) const;
#endif

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

#include "Math/TArithmeticArray.ipp"
