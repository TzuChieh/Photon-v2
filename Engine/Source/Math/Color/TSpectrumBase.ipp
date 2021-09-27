#pragma once

#include "Math/Color/TSpectrumBase.h"
#include "Common/assertion.h"
#include "Math/math.h"
#include "Math/Color/color_spaces.h"

#include <cmath>

namespace ph::math
{

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
inline TSpectrumBase<Derived, COLOR_SPACE, T, N>::TSpectrumBase(const T* const colorValues)
{
	PH_ASSERT(colorValues);

	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] = colorValues[i];
	}
}

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
inline consteval EColorSpace TSpectrumBase<Derived, COLOR_SPACE, T, N>::getColorSpace() noexcept
{
	return COLOR_SPACE;
}

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
inline std::size_t TSpectrumBase<Derived, COLOR_SPACE, T, N>::minComponent() const
{
	// minIndex() is not exposed; use "this" to access it in current scope
	return this->minIndex();
}

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
inline std::size_t TSpectrumBase<Derived, COLOR_SPACE, T, N>::maxComponent() const
{
	// maxIndex() is not exposed; use "this" to access it in current scope
	return this->maxIndex();
}

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
inline auto TSpectrumBase<Derived, COLOR_SPACE, T, N>::setColorValues(const TRawColorValues<T, N>& colorValues)
-> Derived&
{
	// set() is not exposed; use "this" to access it in current scope
	return this->set(colorValues);
}

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
inline auto TSpectrumBase<Derived, COLOR_SPACE, T, N>::getColorValues() const
-> const TRawColorValues<T, N>&
{
	return m;
}

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
template<EColorSpace SRC_COLOR_SPACE>
inline auto TSpectrumBase<Derived, COLOR_SPACE, T, N>::setTransformed(const auto& srcColorValues, const EColorUsage usage)
-> Derived&
{
	static_assert(CColorTransformInterface<Derived>);

	const auto transformedColorValues = transform_color<SRC_COLOR_SPACE, COLOR_SPACE, T>(
		srcColorValues, usage);
	setColorValues(transformedColorValues);

	return static_cast<Derived&>(*this);
}

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
inline T TSpectrumBase<Derived, COLOR_SPACE, T, N>::relativeLuminance(const EColorUsage usage) const
{
	return relative_luminance<COLOR_SPACE, T>(getColorValues(), usage);
}

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
inline auto TSpectrumBase<Derived, COLOR_SPACE, T, N>::setLinearSRGB(const TRawColorValues<T, 3>& linearSRGB, const EColorUsage usage)
-> Derived&
{
	static_assert(CColorTransformInterface<Derived>);

	const auto transformedColorValues = transform_from_linear_sRGB<COLOR_SPACE, T>(
		linearSRGB, usage);
	setColorValues(transformedColorValues);

	return static_cast<Derived&>(*this);
}

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
inline auto TSpectrumBase<Derived, COLOR_SPACE, T, N>::toLinearSRGB(const EColorUsage usage)
-> TRawColorValues<T, 3>
{
	static_assert(CColorTransformInterface<Derived>);

	return transform_to_linear_sRGB<COLOR_SPACE, T>(
		getColorValues(), usage);
}

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
inline auto TSpectrumBase<Derived, COLOR_SPACE, T, N>::putEnergy(const T energyLevel)
-> Derived&
{
	static_assert(CColorTransformInterface<Derived>);

	// For tristimulus color space, use default spectral color space
	if constexpr(TColorSpaceDefinition<COLOR_SPACE, T>::isTristimulus())
	{
		return setColorValues(
			put_color_energy<COLOR_SPACE, T>(getColorValues(), energyLevel));
	}
	// For spectral color space, use itself
	else
	{
		return setColorValues(
			put_color_energy<COLOR_SPACE, T, COLOR_SPACE>(getColorValues(), energyLevel));
	}
}

}// end namespace ph::math
