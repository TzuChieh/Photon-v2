#pragma once

#include "Math/General/TArithmeticArrayBase.h"
#include "Math/Color/spectrum_fwd.h"
#include "Math/math_fwd.h"
#include "Math/Color/color_enums.h"
#include "Math/Color/color_basics.h"
#include "Utility/utility.h"

#include <Common/compiler.h>

#include <cstddef>
#include <type_traits>
#include <concepts>
#include <vector>

namespace ph::math
{

/*! @brief Base for spectrum implementations.

An important assumption is that spectrum implementations are all color samples that can be linearly
combined. `CColorValuesInterface` can be used for directly access the raw sample values.

@note It is not recommended to add your own field in your spectrum implementation. If you must, make sure
there will not be inconsistent object state (slicing) when @p CColorTransformInterface is being used.
*/
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
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(TSpectrumBase);

public:
	using Base::Base;

	/*! @brief Set color values directly. Calling `setColorValues()` has the same effect.
	This is the most general form that allows type conversion. Basically, all other overloads with
	value inputs is equivalent to calling `setColorValues()`, too (unless otherwise noted).
	*/
	template<typename U>
	explicit TSpectrumBase(const TRawColorValues<U, N>& values);

	inline static constexpr auto NUM_VALUES = Base::NUM_ELEMENTS;

	static consteval EColorSpace getColorSpace() noexcept;

	std::size_t minComponent() const;
	std::size_t maxComponent() const;

	/*! @brief Set and get raw color values directly.
	*/
	///@{
	Derived& setColorValues(const TRawColorValues<T, N>& values);
	Derived& setColorValues(T rawColorValue);
	const TRawColorValues<T, N>& getColorValues() const;
	///@}

	template<EColorSpace SRC_COLOR_SPACE>
	Derived& setTransformed(const auto& srcColorValues, EColorUsage usage);

	template<CColorValuesInterface ImplType>
	Derived& setTransformed(const ImplType& srcColorValues, EColorUsage usage);

	template<EColorSpace DST_COLOR_SPACE>
	auto toTransformed(EColorUsage usage) const;

	void transformFrom(const auto& srcColorValues, EColorSpace srcColorSpace, EColorUsage usage);
	void transformTo(auto* out_dstColorValues, EColorSpace dstColorSpace, EColorUsage usage) const;
	
	/*! @brief Helper for setting spectral values to this spectrum.
	*/
	template<EColorSpace SPECTRAL_COLOR_SPACE = EColorSpace::Spectral>
	Derived& setSpectral(const TSpectralSampleValues<T>& sampleValues, EColorUsage usage);

	/*! @brief Helper for setting spectral values to this spectrum.
	*/
	template<CColorValuesInterface ImplType>
	Derived& setSpectral(const ImplType& sampleValues, EColorUsage usage);

	/*! @brief Helper for getting spectral values from this spectrum.
	*/
	template<EColorSpace SPECTRAL_COLOR_SPACE = EColorSpace::Spectral>
	TSpectralSampleValues<T> toSpectral(EColorUsage usage) const;

	// TODO: overloads for directly take another TSpectrumBase in different space

	T relativeLuminance(EColorUsage usage = EColorUsage::EMR) const;

	/*! @brief Helper for setting linear-sRGB color to this spectrum.
	*/
	Derived& setLinearSRGB(const TRawColorValues<T, 3>& linearSRGB, EColorUsage usage);
	
	/*! @brief Helper for converting this spectrum to linear-sRGB color.
	*/
	TRawColorValues<T, 3> toLinearSRGB(EColorUsage usage) const;

	Derived& putEnergy(T energyLevel);

	// Expose public interfaces. `set()` not exposed since we want to use our better-named setters.

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

#if !PH_COMPILER_HAS_P2468R2
	using Base::operator !=;
#endif

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
		return Derived{rhs}.sub(lhs);
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
	
private:
	/*!
	Similar to `setTransformed()`, except that the method does not fail to compile if the type
	of `srcColorValues` is incompatible with `SRC_COLOR_SPACE` (instead, it is a runtime error if called).
	*/
	template<EColorSpace SRC_COLOR_SPACE>
	void setTransformedIfCompatible(const auto& srcColorValues, EColorUsage usage);

	/*!
	Similar to `toTransformed()`, except that the method does not fail to compile if the type
	of `*out_dstColorValues` is incompatible with `DST_COLOR_SPACE`, (instead, it is a runtime error if called).
	*/
	template<EColorSpace DST_COLOR_SPACE>
	void toTransformedIfCompatible(auto* out_dstColorValues, EColorUsage usage) const;
};

}// end namespace ph::math

#include "Math/Color/TSpectrumBase.ipp"
