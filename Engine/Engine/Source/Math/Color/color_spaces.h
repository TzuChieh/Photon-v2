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
concept CHasColorSpaceProperties = requires
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
Bound tristimulus color space: A spectral color space must choose a tristimulus color space as its 
binding space, which will be used by color space operations that cannot operate in the spectral space.
The chosen color space is often referred to as "bound space" for simplicity.
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
Specialize this class to provide definitions for color space. Must satisfy
`CTristimulusColorSpaceDefinition` or `CSpectralColorSpaceDefinition`.
*/
template<EColorSpace COLOR_SPACE, typename T>
class TColorSpaceDefinition final
{
	// Available color spaces must provide definition and thus should not end up here.
	static_assert(COLOR_SPACE == EColorSpace::Unspecified || COLOR_SPACE == EColorSpace::NUM,
		"No definition for the specified COLOR_SPACE.");

public:
	static consteval EColorSpace getColorSpace() noexcept
	// Should not provide color space info for `EColorSpace::NUM` as it is not even a color space
	requires (COLOR_SPACE == EColorSpace::Unspecified)
	{
		return COLOR_SPACE;
	}

	static consteval bool isTristimulus() noexcept
	{
		return false;
	}
};

/*! @brief Helper alias for @p TColorSpaceDefinition which uses the @p ColorValue type.
*/
template<EColorSpace COLOR_SPACE>
using TColorSpaceDef = TColorSpaceDefinition<COLOR_SPACE, ColorValue>;

/*! @brief Check whether @p colorSpace is a tristimulus color space.
This is a runtime check. For compile-time check, use `TColorSpaceDef<?>::isTristimulus()`.
*/
bool is_tristimulus(EColorSpace colorSpace);

/*! @brief Check whether @p InColorValuesType is suitable to represent values in @p COLOR_SPACE.
*/
template<typename InColorValuesType, EColorSpace COLOR_SPACE>
constexpr bool is_compatible();

/*!
@param srcColorValues A @p TTristimulusValues or a @p TSpectralSampleValues depending on
whether@p SRC_COLOR_SPACE is tristimulus.
@return A @p TTristimulusValues or a @p TSpectralSampleValues depending on whether @p DST_COLOR_SPACE
is tristimulus.
*/
template<EColorSpace SRC_COLOR_SPACE, EColorSpace DST_COLOR_SPACE, typename T, EChromaticAdaptation ALGORITHM = EChromaticAdaptation::Default>
auto transform_color(const auto& srcColorValues, EColorUsage usage);

/*!
@return Relative luminance of @p srcColorValues. Normally for a tristimulus color, its reference white's
relative luminance will be 1. For spectral samples, it is the result of performing an inner product
with the luminous efficiency function. For ECF usages, its relative luminance is guaranteed to be 
within [0, 1].
*/
template<EColorSpace SRC_COLOR_SPACE, typename T, EChromaticAdaptation ALGORITHM = EChromaticAdaptation::Default>
T relative_luminance(const auto& srcColorValues, EColorUsage usage = EColorUsage::EMR);

template<EColorSpace SRC_COLOR_SPACE, typename T, EColorSpace SPECTRAL_COLOR_SPACE = EColorSpace::Spectral, EChromaticAdaptation ALGORITHM = EChromaticAdaptation::Default>
T estimate_color_energy(const auto& srcColorValues);

template<EColorSpace SRC_COLOR_SPACE, typename T, EColorSpace SPECTRAL_COLOR_SPACE = EColorSpace::Spectral, EChromaticAdaptation ALGORITHM = EChromaticAdaptation::Default>
auto normalize_color_energy(const auto& srcColorValues);

template<EColorSpace SRC_COLOR_SPACE, typename T, EColorSpace SPECTRAL_COLOR_SPACE = EColorSpace::Spectral, EChromaticAdaptation ALGORITHM = EChromaticAdaptation::Default>
auto put_color_energy(const auto& srcColorValues, T energyLevel);

/*! @brief Helper for converting from sRGB to linear-sRGB.
*/
template<typename T>
TTristimulusValues<T> sRGB_nonlinear_to_linear(const TTristimulusValues<T>& nonlinearSRGB);

/*! @brief Helper for converting from linear-sRGB to sRGB.
*/
template<typename T>
TTristimulusValues<T> sRGB_linear_to_nonlinear(const TTristimulusValues<T>& linearSRGB);

/*! @brief Helper for converting from sRGB to any color space.
*/
template<EColorSpace DST_COLOR_SPACE, typename T>
auto transform_from_sRGB(const TTristimulusValues<T>& nonlinearSRGB, EColorUsage usage);

/*! @brief Helper for converting from linear-sRGB to any color space.
*/
template<EColorSpace DST_COLOR_SPACE, typename T>
auto transform_from_linear_sRGB(const TTristimulusValues<T>& linearSRGB, EColorUsage usage);

/*! @brief Helper for converting any color space to sRGB.
*/
template<EColorSpace SRC_COLOR_SPACE, typename T>
TTristimulusValues<T> transform_to_sRGB(const auto& srcColorValues, EColorUsage usage);

/*! @brief Helper for converting any color space to linear-sRGB.
*/
template<EColorSpace SRC_COLOR_SPACE, typename T>
TTristimulusValues<T> transform_to_linear_sRGB(const auto& srcColorValues, EColorUsage usage);

}// end namespace ph::math

#include "Math/Color/color_spaces.ipp"
