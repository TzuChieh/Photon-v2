#pragma once

#include "Math/General/TArithmeticArrayBase.h"
#include "Math/math_fwd.h"
#include "Math/Color/EColorSpace.h"

#include <cstddef>
#include <type_traits>
#include <concepts>

namespace ph::math
{

template<typename Derived, typename ColorType, EColorSpace COLOR_SPACE>
concept CHasSpectrumImplementations = requires (
	Derived&       nonConstDerived, 
	const Derived& constDerived, 
	ColorType      inputColor)
{
	{ nonConstDerived.template impl_from<COLOR_SPACE>(inputColor) };
	{ constDerived.template impl_to<COLOR_SPACE>() } -> std::same_as<ColorType>;
};

template<typename Derived, typename T, std::size_t N>
class TSpectrumBase : private TArithmeticArrayBase<Derived, T, N>
{
private:
	using Base = TArithmeticArrayBase<Derived, T, N>;

	// Required as base class need access to protected ctors
	friend Base;

protected:
	using Base::m;

// Hide special members as this class is not intended to be used polymorphically.
// It is derived class's choice to expose them (by defining them in public) or not.
protected:
	inline TSpectrumBase() = default;
	inline TSpectrumBase(const TSpectrumBase& other) = default;
	inline TSpectrumBase(TSpectrumBase&& other) = default;
	inline TSpectrumBase& operator = (const TSpectrumBase& rhs) = default;
	inline TSpectrumBase& operator = (TSpectrumBase&& rhs) = default;
	inline ~TSpectrumBase() = default;

public:
	using Base::Base;

	template<typename U>
	explicit TSpectrumBase(const std::array<U, N>& values);

	template<typename U>
	explicit TSpectrumBase(const TArithmeticArray<U, N>& values);

	template<typename U>
	explicit TSpectrumBase(const U* values);

	template<typename U>
	explicit TSpectrumBase(const std::vector<U>& values);

	std::size_t minComponent() const;
	std::size_t maxComponent() const;

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

#include "Math/Color/TSpectrumBase.ipp"
