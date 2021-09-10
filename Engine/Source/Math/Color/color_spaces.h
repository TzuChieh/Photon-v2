#pragma once

#include "Math/Color/EColorSpace.h"
#include "Math/Color/EColorUsage.h"
#include "Math/Color/EReferenceWhite.h"
#include "Common/primitive_type.h"
#include "Common/config.h"
#include "Utility/IUninstantiable.h"

#include <concepts>
#include <array>
#include <type_traits>

namespace ph::math
{

template<typename T>
concept CColorSpaceDefinition = CHasColorSpaceProperties<T>;

template<EColorSpace COLOR_SPACE, EReferenceWhite REFERENCE_WHITE>
class TColorSpaceDefinitionHelper : private IUninstantiable
{
protected:
	inline static constexpr EColorSpace getColorSpace() noexcept
	{
		return COLOR_SPACE;
	}

	inline static constexpr EReferenceWhite getReferenceWhite() noexcept
	{
		return REFERENCE_WHITE;
	}
};

template<EColorSpace COLOR_SPACE, EReferenceWhite REFERENCE_WHITE>
class TTristimulusColorSpaceDefinitionHelper : public TColorSpaceDefinitionHelper<COLOR_SPACE, REFERENCE_WHITE>
{
private:
	using Base = TColorSpaceDefinitionHelper<COLOR_SPACE, REFERENCE_WHITE>;

public:
	inline static constexpr bool isTristimulus() noexcept
	{
		return true;
	}

	using Base::getColorSpace;
	using Base::getReferenceWhite;
};

template<EColorSpace COLOR_SPACE, EReferenceWhite REFERENCE_WHITE, EColorSpace BOUND_TRISTIMULUS_COLOR_SPACE>
class TSpectralColorSpaceDefinitionHelper : public TColorSpaceDefinitionHelper<COLOR_SPACE, REFERENCE_WHITE>
{
	static_assert(COLOR_SPACE != BOUND_TRISTIMULUS_COLOR_SPACE,
		"Cannot define a spectral space that binds itself as a tristimulus one. "
		"A color space can be either spectral or tristimulus but not both.");

private:
	using Base = TColorSpaceDefinitionHelper<COLOR_SPACE, REFERENCE_WHITE>;

public:
	inline static constexpr bool isTristimulus() noexcept
	{
		return false;
	}

	using Base::getColorSpace;
	using Base::getReferenceWhite;
};

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
	{ ColorSpaceDefType::toCIEXYZ(thisColor) } -> std::same_as<TristimulusValues>;
	{ ColorSpaceDefType::fromCIEXYZ(CIEXYZColor) } -> std::same_as<TristimulusValues>;
};

template<typename ColorSpaceDefType>
concept CSupportsSpectralConversions = requires (TristimulusValues boundColor, SpectralSampleValues sampleValues)
{
	{ ColorSpaceDefType::getBoundTristimulusColorSpace() } -> std::same_as<EColorSpace>;
	{ ColorSpaceDefType::upSample(boundColor) } -> std::same_as<SpectralSampleValues>;
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
