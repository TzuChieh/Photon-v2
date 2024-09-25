#pragma once

#include "Math/Color/TSpectrumBase.h"
#include "Math/math.h"
#include "Math/Color/color_spaces.h"
#include "Math/math_exceptions.h"

#include <Common/assertion.h>

#include <cmath>

namespace ph::math
{

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
template<typename U>
inline TSpectrumBase<Derived, COLOR_SPACE, T, N>::TSpectrumBase(const TRawColorValues<U, N>& values)
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] = static_cast<T>(values[i]);
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
inline auto TSpectrumBase<Derived, COLOR_SPACE, T, N>::setColorValues(const TRawColorValues<T, N>& values)
-> Derived&
{
	// set() is not exposed; use "this" to access it in current scope
	return this->set(values);
}

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
inline auto TSpectrumBase<Derived, COLOR_SPACE, T, N>::setColorValues(const T rawColorValues)
-> Derived&
{
	// set() is not exposed; use "this" to access it in current scope
	return this->set(rawColorValues);
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
	const auto transformedColorValues = transform_color<SRC_COLOR_SPACE, COLOR_SPACE, T>(
		srcColorValues, usage);
	setColorValues(transformedColorValues);

	return static_cast<Derived&>(*this);
}

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
template<CColorValuesInterface ImplType>
inline auto TSpectrumBase<Derived, COLOR_SPACE, T, N>::setTransformed(const ImplType& srcColorValues, const EColorUsage usage)
-> Derived&
{
	return setTransformed<ImplType::getColorSpace()>(
		srcColorValues.getColorValues(), usage);
}

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
template<EColorSpace SPECTRAL_COLOR_SPACE>
inline auto TSpectrumBase<Derived, COLOR_SPACE, T, N>::setSpectral(const TSpectralSampleValues<T>& sampleValues, const EColorUsage usage)
-> Derived&
{
	static_assert(!TColorSpaceDefinition<SPECTRAL_COLOR_SPACE, T>::isTristimulus());

	return setTransformed<SPECTRAL_COLOR_SPACE>(sampleValues, usage);
}

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
template<CColorValuesInterface ImplType>
inline auto TSpectrumBase<Derived, COLOR_SPACE, T, N>::setSpectral(const ImplType& sampleValues, const EColorUsage usage)
-> Derived&
{
	return setSpectral<ImplType::getColorSpace()>(
		sampleValues.getColorValues(), usage);
}

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
template<EColorSpace DST_COLOR_SPACE>
inline auto TSpectrumBase<Derived, COLOR_SPACE, T, N>::toTransformed(const EColorUsage usage) const
{
	return transform_color<COLOR_SPACE, DST_COLOR_SPACE, T>(
		getColorValues(), usage);
}

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
template<EColorSpace DST_COLOR_SPACE>
inline TSpectralSampleValues<T> TSpectrumBase<Derived, COLOR_SPACE, T, N>::toSpectral(const EColorUsage usage) const
{
	return toTransformed<DST_COLOR_SPACE>(usage);
}

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
inline T TSpectrumBase<Derived, COLOR_SPACE, T, N>::relativeLuminance(const EColorUsage usage) const
{
	return relative_luminance<COLOR_SPACE, T>(getColorValues(), usage);
}

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
template<EColorSpace SRC_COLOR_SPACE>
inline void TSpectrumBase<Derived, COLOR_SPACE, T, N>::setTransformedIfCompatible(
	const auto& srcColorValues, const EColorUsage usage)
{
	if constexpr(is_compatible<decltype(srcColorValues), SRC_COLOR_SPACE>)
	{
		setTransformed<SRC_COLOR_SPACE>(srcColorValues, usage);
	}
	else
	{
		throw ColorError(
			"Using incompatible type for color transformation (`srcColorValues` is incompatible "
			"with its color space).");
	}
}

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
template<EColorSpace DST_COLOR_SPACE>
inline void TSpectrumBase<Derived, COLOR_SPACE, T, N>::toTransformedIfCompatible(
	auto* const out_dstColorValues, const EColorUsage usage) const
{
	PH_ASSERT(out_dstColorValues);

	if constexpr(is_compatible<decltype(*out_dstColorValues), DST_COLOR_SPACE>())
	{
		*out_dstColorValues = toTransformed<DST_COLOR_SPACE>(usage);
	}
	else
	{
		throw ColorError(
			"Using incompatible type for color transformation (`out_dstColorValues` is incompatible "
			"with its color space).");
	}
}

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
inline void TSpectrumBase<Derived, COLOR_SPACE, T, N>::transformFrom(
	const auto& srcColorValues, const EColorSpace srcColorSpace, const EColorUsage usage)
{
	switch(srcColorSpace)
	{
	case EColorSpace::CIE_XYZ:
		return setTransformedIfCompatible<EColorSpace::CIE_XYZ>(srcColorValues, usage);

	case EColorSpace::CIE_xyY:
		return setTransformedIfCompatible<EColorSpace::CIE_xyY>(srcColorValues, usage);

	case EColorSpace::Linear_sRGB:
		return setTransformedIfCompatible<EColorSpace::Linear_sRGB>(srcColorValues, usage);

	case EColorSpace::sRGB:
		return setTransformedIfCompatible<EColorSpace::sRGB>(srcColorValues, usage);

	case EColorSpace::ACEScg:
		return setTransformedIfCompatible<EColorSpace::ACEScg>(srcColorValues, usage);

	case EColorSpace::Spectral_Smits:
		return setTransformedIfCompatible<EColorSpace::Spectral_Smits>(srcColorValues, usage);

	default:
		// When failed, you may have added/removed some entries. Handle it in the above section.
		PH_ASSERT_UNREACHABLE_SECTION();
		setColorValues(0);
		break;
	}
}

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
inline void TSpectrumBase<Derived, COLOR_SPACE, T, N>::transformTo(
	auto* const out_dstColorValues, const EColorSpace dstColorSpace, const EColorUsage usage) const
{
	PH_ASSERT(out_dstColorValues);

	switch(dstColorSpace)
	{
	case EColorSpace::CIE_XYZ:
		return toTransformedIfCompatible<EColorSpace::CIE_XYZ>(out_dstColorValues, usage);

	case EColorSpace::CIE_xyY:
		return toTransformedIfCompatible<EColorSpace::CIE_xyY>(out_dstColorValues, usage);

	case EColorSpace::Linear_sRGB:
		return toTransformedIfCompatible<EColorSpace::Linear_sRGB>(out_dstColorValues, usage);

	case EColorSpace::sRGB:
		return toTransformedIfCompatible<EColorSpace::sRGB>(out_dstColorValues, usage);

	case EColorSpace::ACEScg:
		return toTransformedIfCompatible<EColorSpace::ACEScg>(out_dstColorValues, usage);

	case EColorSpace::Spectral_Smits:
		return toTransformedIfCompatible<EColorSpace::Spectral_Smits>(out_dstColorValues, usage);

	default:
		// When failed, you may have added/removed some entries. Handle it in the above section.
		PH_ASSERT_UNREACHABLE_SECTION();
		out_dstColorValues->fill(0);
		break;
	}
}

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
inline auto TSpectrumBase<Derived, COLOR_SPACE, T, N>::setLinearSRGB(const TRawColorValues<T, 3>& linearSRGB, const EColorUsage usage)
-> Derived&
{
	static_assert(CColorValuesInterface<Derived>);

	const auto transformedColorValues = transform_from_linear_sRGB<COLOR_SPACE, T>(
		linearSRGB, usage);
	setColorValues(transformedColorValues);

	return static_cast<Derived&>(*this);
}

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
inline auto TSpectrumBase<Derived, COLOR_SPACE, T, N>::toLinearSRGB(const EColorUsage usage) const
-> TRawColorValues<T, 3>
{
	static_assert(CColorValuesInterface<Derived>);

	return transform_to_linear_sRGB<COLOR_SPACE, T>(
		getColorValues(), usage);
}

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
inline auto TSpectrumBase<Derived, COLOR_SPACE, T, N>::putEnergy(const T energyLevel)
-> Derived&
{
	static_assert(CColorValuesInterface<Derived>);

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

#if PH_COMPILER_HAS_P2468R2

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
inline bool TSpectrumBase<Derived, COLOR_SPACE, T, N>::operator == (const Self& other) const
{
	return Base::operator == (other);
}

#endif

}// end namespace ph::math
