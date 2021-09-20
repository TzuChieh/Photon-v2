#pragma once

#include "Math/General/TArithmeticArrayBase.h"
#include "Math/Color/spectrum_fwd.h"
#include "Math/math_fwd.h"
#include "Math/Color/color_basics.h"

#include <cstddef>
#include <type_traits>
#include <concepts>
#include <vector>

namespace ph::math
{

namespace detail
{

template<EColorSpace COLOR_SPACE>
class TColorSpaceDummy final
{};

}// end namespace detail

template<typename ImplType>
concept CColorTransformInterface = requires (
	ImplType       instance,
	const ImplType constInstance)
{
	// getColorSpace() must be usable as a non-type template argument
	detail::TColorSpaceDummy<ImplType::getColorSpace()>();

	// Test for the method void setColorValues(const TRawColorValues<T, N>&).
	// Note: we do not want to pass in the template parameters (for convenience), so we test by calling 
	// the method with an empty braced-init-list.
	instance.setColorValues({});

	// Test for the method getColorValues().
	// Note: we do not want to pass in the template parameters (for convenience), so we test by calling 
	// the method and see if the return value can be subscripted.
	constInstance.getColorValues()[0];
};

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
class TSpectrumBase : public TArithmeticArrayBase<Derived, T, N>
{
private:
	using Base = TArithmeticArrayBase<Derived, T, N>;

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
	explicit TSpectrumBase(const TRawColorValues<U, N>& colorValues);

	template<typename U>
	explicit TSpectrumBase(const U* colorValues);

	template<typename U>
	explicit TSpectrumBase(const std::vector<U>& colorValues);

	static consteval EColorSpace getColorSpace() noexcept;

	std::size_t minComponent() const;
	std::size_t maxComponent() const;

	Derived& setColorValues(const TRawColorValues<T, N>& colorValues);
	const TRawColorValues<T, N>& getColorValues() const;

	template<typename OtherSpectrum, EChromaticAdaptation ALGORITHM = EChromaticAdaptation::Bradford>
	Derived& setTransformed(const OtherSpectrum& otherSpectrum, EColorUsage usage = EColorUsage::UNSPECIFIED);

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
