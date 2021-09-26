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
template<typename OtherSpectrum, EChromaticAdaptation ALGORITHM>
inline auto TSpectrumBase<Derived, COLOR_SPACE, T, N>::setTransformed(const OtherSpectrum& otherSpectrum, const EColorUsage usage)
-> Derived&
{
	static_assert(CColorTransformInterface<Derived>);
	static_assert(CColorTransformInterface<OtherSpectrum>);

	const auto transformedColorValues = transform_color<COLOR_SPACE, OtherSpectrum::getColorSpace(), T, ALGORITHM>(
			otherSpectrum.getColorValues(), usage);

	setColorValues(transformedColorValues);

	return static_cast<Derived&>(*this);
}

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
template<EChromaticAdaptation ALGORITHM>
inline T TSpectrumBase<Derived, COLOR_SPACE, T, N>::relativeLuminance(const EColorUsage usage) const
{
	return relative_luminance<COLOR_SPACE, T, ALGORITHM>(getColorValues(), usage);
}

}// end namespace ph::math
