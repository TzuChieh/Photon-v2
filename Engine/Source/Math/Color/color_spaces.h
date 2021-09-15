#pragma once

#include "Math/Color/color_basics.h"
#include "Utility/IUninstantiable.h"

#include <concepts>
#include <type_traits>
#include <cmath>

namespace ph::math
{

/*! @brief Basic requirements for a color space definition.
*/
template<typename T>
concept CHasColorSpaceProperties = requires ()
{
	{ T::getColorSpace() } noexcept -> std::same_as<EColorSpace>;
	{ T::getReferenceWhite() } noexcept -> std::same_as<EReferenceWhite>;
	{ T::isTristimulus() } noexcept -> std::same_as<bool>;
};

/*! @brief Basic requirements a tristimulus color space definition must satisfy in addition to CHasColorSpaceProperties.
*/
template<typename T>
concept CSupportsTristimulusConversions = requires (TristimulusValues thisColor, TristimulusValues CIEXYZColor)
{
	{ T::toCIEXYZ(thisColor) } -> std::same_as<TristimulusValues>;
	{ T::fromCIEXYZ(CIEXYZColor) } -> std::same_as<TristimulusValues>;
};

/*! @brief Basic requirements a spectral color space definition must satisfy in addition to CHasColorSpaceProperties.
*/
template<typename T>
concept CSupportsSpectralConversions = requires (TristimulusValues boundColor, SpectralSampleValues sampleValues)
{
	{ T::getBoundTristimulusColorSpace() } -> std::same_as<EColorSpace>;
	{ T::upSample(boundColor) } -> std::same_as<SpectralSampleValues>;
	{ T::downSample(sampleValues) } -> std::same_as<TristimulusValues>;
};

template<typename T>
concept CColorSpaceDefinition = CHasColorSpaceProperties<T>;

template<typename T>
concept CTristimulusColorSpaceDefinition = 
	CColorSpaceDefinition<T> && 
	CSupportsTristimulusConversions<T>;

template<typename T>
concept CSpectralColorSpaceDefinition = 
	CColorSpaceDefinition<T> && 
	CSupportsSpectralConversions<T>;

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

	inline static constexpr EColorSpace getBoundTristimulusColorSpace() noexcept
	{
		return BOUND_TRISTIMULUS_COLOR_SPACE;
	}

	using Base::getColorSpace;
	using Base::getReferenceWhite;
};

/*! @brief Sinkhole for color spaces without definition.
Specialize the class to provide definitions for color space. Must satisfy CTristimulusColorSpaceDefinition or
CSpectralColorSpaceDefinition.
*/
template<EColorSpace COLOR_SPACE>
class TColorSpaceDefinition final
{
	// Available color spaces must provide definition and thus should not end up here.
	static_assert(COLOR_SPACE == EColorSpace::UNSPECIFIED,
		"No definition for the specified COLOR_SPACE.");
};

// Unspecified color space must be neither tristimulus nor spectral.
static_assert(!CTristimulusColorSpaceDefinition<TColorSpaceDefinition<EColorSpace::UNSPECIFIED>>);
static_assert(!CSpectralColorSpaceDefinition<TColorSpaceDefinition<EColorSpace::UNSPECIFIED>>);

}// end namespace ph::math

#include "Math/Color/color_spaces.ipp"
