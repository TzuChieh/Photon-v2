#pragma once

#include "Math/Color/EColorSpace.h"
#include "Math/Color/EColorUsage.h"
#include "Math/Color/EReferenceWhite.h"
#include "Common/primitive_type.h"
#include "Common/config.h"

#include <concepts>
#include <array>
#include <type_traits>

namespace ph::math
{

template<typename T>
concept CColorSpaceDefinition = CHasColorSpaceProperties<T>;

template<EColorSpace COLOR_SPACE, EReferenceWhite REFERENCE_WHITE, bool IS_TRISTIMULUS>
class TColorSpaceDefinitionBase
{
public:
	inline static constexpr EColorSpace getColorSpace() noexcept
	{
		return COLOR_SPACE;
	}

	inline static constexpr EReferenceWhite getReferenceWhite() noexcept
	{
		return REFERENCE_WHITE;
	}

	inline static constexpr bool isTristimulus() noexcept
	{
		return IS_TRISTIMULUS;
	}

// Hide special members as this class is not intended to be used polymorphically.
protected:
	inline TColorSpaceDefinitionBase() = default;
	inline TColorSpaceDefinitionBase(const TColorSpaceDefinitionBase& other) = default;
	inline TColorSpaceDefinitionBase(TColorSpaceDefinitionBase&& other) = default;
	inline TColorSpaceDefinitionBase& operator = (const TColorSpaceDefinitionBase& rhs) = default;
	inline TColorSpaceDefinitionBase& operator = (TColorSpaceDefinitionBase&& rhs) = default;
	inline ~TColorSpaceDefinitionBase() = default;
};

template<EColorSpace COLOR_SPACE, EReferenceWhite REFERENCE_WHITE>
using TTristimulusColorSpaceDefinition = TColorSpaceDefinitionBase<COLOR_SPACE, REFERENCE_WHITE, true>;

template<EColorSpace COLOR_SPACE, EReferenceWhite REFERENCE_WHITE>
using TSpectralColorSpaceDefinition = TColorSpaceDefinitionBase<COLOR_SPACE, REFERENCE_WHITE, false>;

template<EColorSpace COLOR_SPACE>
class TColorSpaceDefinition final
{
	static_assert(COLOR_SPACE == EColorSpace::UNSPECIFIED,
		"No definition for the specified COLOR_SPACE.");
};

using TristimulusValues    = std::array<real, 3>;
using SpectralSampleValues = std::array<real, PH_SPECTRUM_SAMPLED_NUM_SAMPLES>;

template<typename ColorSpaceDefType>
concept CHasColorSpaceProperties = requires ()
{
	{ ColorSpaceDefType::getColorSpace() } noexcept -> std::same_as<EColorSpace>;
	{ ColorSpaceDefType::getReferenceWhite() } noexcept -> std::same_as<EReferenceWhite>;
	{ ColorSpaceDefType::isTristimulus() } noexcept -> std::same_as<bool>;
};

template<typename ColorSpaceDefType>
concept CSupportsTristimulusConversions = requires (TristimulusValues thisColor, TristimulusValues CIEXYZColor)
{
	{ ColorSpaceDefType::toCIEXYZ(thisColor) } noexcept -> std::same_as<TristimulusValues>;
	{ ColorSpaceDefType::fromCIEXYZ(CIEXYZColor) } noexcept -> std::same_as<TristimulusValues>;
};

template<typename ColorSpaceDefType>
concept CSupportsSpectralConversions = requires (TristimulusValues thisColor, SpectralSampleValues sampleValues)
{
	{ ColorSpaceDefType::upSample(thisColor) } -> std::same_as<SpectralSampleValues>;
	{ ColorSpaceDefType::downSample(sampleValues) } -> std::same_as<TristimulusValues>;
};

template<CHasColorSpaceProperties ColorSpaceDefType>
using TColorValues = std::conditional_t<ColorSpaceDefType::isTristimulus(), TristimulusValues, SpectralSampleValues>;

template<typename ColorSpaceDefType, EColorSpace FROM_COLOR_SPACE>
concept CSupportsDirectConversionFrom = requires (TColorValues<FromColorSpaceDefType> fromColor)
{
	{ ColorSpaceDefType::template directlyFrom<FromColorSpaceDefType::getColorSpace()>(fromColor) } -> std::same_as<TColorValues<ColorSpaceDefType>>;
};



template<>
class TColorSpaceDefinition<EColorSpace::CIE_XYZ> final
{
public:
	inline static bool isTristimulus()
	{
		return true;
	}

	inline static EReferenceWhite getReferenceWhite()
	{
		return EReferenceWhite::D65;
	}

	inline static Vector3R toCIEXYZ(const Vector3R& thisColor)
	{
		// Already in CIE-XYZ space, simply return the source color.
		return thisColor;
	}

	inline static Vector3R fromCIEXYZ(const Vector3R& CIEXYZColor)
	{
		// Already in CIE-XYZ space, simply return the source color.
		return CIEXYZColor;
	}
};

}// end namespace ph::math
