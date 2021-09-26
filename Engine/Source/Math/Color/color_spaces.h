#pragma once

#include "Math/Color/color_enums.h"
#include "Math/Color/color_basics.h"

#include <concepts>
#include <type_traits>

namespace ph::math
{

/*! @brief Basic requirements for a color space definition.
*/
template<typename DefType>
concept CHasColorSpaceProperties = requires ()
{
	{ DefType::getColorSpace() } noexcept -> std::same_as<EColorSpace>;
	{ DefType::isTristimulus() } noexcept -> std::same_as<bool>;
};

/*! @brief Basic requirements a tristimulus color space definition must satisfy in addition to CHasColorSpaceProperties.
*/
template<typename DefType, typename T>
concept CSupportsTristimulusConversions = requires (TTristimulusValues<T> thisColor, TTristimulusValues<T> CIEXYZColor)
{
	{ DefType::getReferenceWhite() } noexcept -> std::same_as<EReferenceWhite>;
	{ DefType::toCIEXYZ(thisColor) } -> std::same_as<TTristimulusValues<T>>;
	{ DefType::fromCIEXYZ(CIEXYZColor) } -> std::same_as<TTristimulusValues<T>>;
};

/*! @brief Basic requirements a spectral color space definition must satisfy in addition to CHasColorSpaceProperties.
*/
template<typename DefType, typename T>
concept CSupportsSpectralConversions = requires (
	TTristimulusValues<T>    boundColor, 
	TSpectralSampleValues<T> sampleValues,
	EColorUsage              usage)
{
	{ DefType::getBoundTristimulusColorSpace() } -> std::same_as<EColorSpace>;
	{ DefType::upSample(boundColor, usage) } -> std::same_as<TSpectralSampleValues<T>>;
	{ DefType::downSample(sampleValues, usage) } -> std::same_as<TTristimulusValues<T>>;
};

template<typename DefType, typename T>
concept CTristimulusColorSpaceDefinition = 
	CHasColorSpaceProperties<DefType> &&
	CSupportsTristimulusConversions<DefType, T>;

template<typename DefType, typename T>
concept CSpectralColorSpaceDefinition = 
	CHasColorSpaceProperties<DefType> &&
	CSupportsSpectralConversions<DefType, T>;

template<typename DefType, typename T>
concept CColorSpaceDefinition = 
	CTristimulusColorSpaceDefinition<DefType, T> ||
	CSpectralColorSpaceDefinition<DefType, T>;

/*! @brief Sinkhole for color spaces without definition.
Specialize the class to provide definitions for color space. Must satisfy CTristimulusColorSpaceDefinition or
CSpectralColorSpaceDefinition.
*/
template<EColorSpace COLOR_SPACE, typename T>
class TColorSpaceDefinition final
{
	// Available color spaces must provide definition and thus should not end up here.
	static_assert(COLOR_SPACE == EColorSpace::UNSPECIFIED || COLOR_SPACE == EColorSpace::NUM,
		"No definition for the specified COLOR_SPACE.");
};

/*!
@param srcColorValues A @p TTristimulusValues or a @p TSpectralSampleValues depending on whether @p SRC_COLOR_SPACE
is tristimulus.
@return A @p TTristimulusValues or a @p TSpectralSampleValues depending on whether @p DST_COLOR_SPACE
is tristimulus.
*/
template<EColorSpace SRC_COLOR_SPACE, EColorSpace DST_COLOR_SPACE, typename T, EChromaticAdaptation ALGORITHM = EChromaticAdaptation::Bradford>
auto transform_color(const auto& srcColorValues, EColorUsage usage);

/*!
@return Relative luminance of @p srcColorValues. Normally for a tristimulus color, its reference white's
relative luminance will be 1. For spectral samples, it is the result of performing an inner product with the
luminous efficiency function. For ECF usages, its relative luminance is guaranteed to be within [0, 1].
*/
template<EColorSpace SRC_COLOR_SPACE, typename T, EChromaticAdaptation ALGORITHM = EChromaticAdaptation::Bradford>
T relative_luminance(const auto& srcColorValues, EColorUsage usage);

template<EColorSpace SRC_COLOR_SPACE, typename T, EColorSpace SPECTRAL_COLOR_SPACE = EColorSpace::Spectral_Smits, EChromaticAdaptation ALGORITHM = EChromaticAdaptation::Bradford>
T estimate_color_energy(const auto& srcColorValues, EColorUsage usage);

template<EColorSpace SRC_COLOR_SPACE, typename T, EColorSpace SPECTRAL_COLOR_SPACE = EColorSpace::Spectral_Smits, EChromaticAdaptation ALGORITHM = EChromaticAdaptation::Bradford>
auto normalize_color_energy(const auto& srcColorValues, EColorUsage usage);

template<typename T>
TTristimulusValues<T> sRGB_nonlinear_to_linear(const TTristimulusValues<T>& nonlinearSRGB);

template<typename T>
TTristimulusValues<T> sRGB_linear_to_nonlinear(const TTristimulusValues<T>& linearSRGB);

template<EColorSpace DST_COLOR_SPACE, typename T>
auto transform_from_sRGB(const TTristimulusValues<T>& nonlinearSRGB, EColorUsage usage);

template<EColorSpace DST_COLOR_SPACE, typename T>
auto transform_from_linear_sRGB(const TTristimulusValues<T>& linearSRGB, EColorUsage usage);

template<EColorSpace SRC_COLOR_SPACE, typename T>
TTristimulusValues<T> transform_to_sRGB(const auto& srcColorValues, EColorUsage usage);

template<EColorSpace SRC_COLOR_SPACE, typename T>
TTristimulusValues<T> transform_to_linear_sRGB(const auto& srcColorValues, EColorUsage usage);

}// end namespace ph::math

#include "Math/Color/color_spaces.ipp"
