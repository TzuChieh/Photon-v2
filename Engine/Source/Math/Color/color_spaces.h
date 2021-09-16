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
template<typename DefType>
concept CHasColorSpaceProperties = requires ()
{
	{ DefType::getColorSpace() } noexcept -> std::same_as<EColorSpace>;
	{ DefType::getReferenceWhite() } noexcept -> std::same_as<EReferenceWhite>;
	{ DefType::isTristimulus() } noexcept -> std::same_as<bool>;
};

/*! @brief Basic requirements a tristimulus color space definition must satisfy in addition to CHasColorSpaceProperties.
*/
template<typename DefType>
concept CSupportsTristimulusConversions = requires (TristimulusValues thisColor, TristimulusValues CIEXYZColor)
{
	{ DefType::toCIEXYZ(thisColor) } -> std::same_as<TristimulusValues>;
	{ DefType::fromCIEXYZ(CIEXYZColor) } -> std::same_as<TristimulusValues>;
};

/*! @brief Basic requirements a spectral color space definition must satisfy in addition to CHasColorSpaceProperties.
*/
template<typename DefType>
concept CSupportsSpectralConversions = requires (TristimulusValues boundColor, SpectralSampleValues sampleValues)
{
	{ DefType::getBoundTristimulusColorSpace() } -> std::same_as<EColorSpace>;
	{ DefType::upSample(boundColor) } -> std::same_as<SpectralSampleValues>;
	{ DefType::downSample(sampleValues) } -> std::same_as<TristimulusValues>;
};

template<typename DefType>
concept CColorSpaceDefinition = CHasColorSpaceProperties<DefType>;

template<typename DefType>
concept CTristimulusColorSpaceDefinition = 
	CColorSpaceDefinition<DefType> &&
	CSupportsTristimulusConversions<DefType>;

template<typename DefType>
concept CSpectralColorSpaceDefinition = 
	CColorSpaceDefinition<DefType> &&
	CSupportsSpectralConversions<DefType>;

template<EColorSpace COLOR_SPACE, EReferenceWhite REF_WHITE>
class TColorSpaceDefinitionHelper : private IUninstantiable
{
protected:
	inline static constexpr EColorSpace getColorSpace() noexcept
	{
		return COLOR_SPACE;
	}

	inline static constexpr EReferenceWhite getReferenceWhite() noexcept
	{
		return REF_WHITE;
	}
};

template<EColorSpace COLOR_SPACE, EReferenceWhite REF_WHITE>
class TTristimulusColorSpaceDefinitionHelper : public TColorSpaceDefinitionHelper<COLOR_SPACE, REF_WHITE>
{
private:
	using Base = TColorSpaceDefinitionHelper<COLOR_SPACE, REF_WHITE>;

public:
	inline static constexpr bool isTristimulus() noexcept
	{
		return true;
	}

	using Base::getColorSpace;
	using Base::getReferenceWhite;
};

template<EColorSpace COLOR_SPACE, EReferenceWhite REF_WHITE, EColorSpace BOUND_TRISTIMULUS_COLOR_SPACE>
class TSpectralColorSpaceDefinitionHelper : public TColorSpaceDefinitionHelper<COLOR_SPACE, REF_WHITE>
{
	static_assert(COLOR_SPACE != BOUND_TRISTIMULUS_COLOR_SPACE,
		"Cannot define a spectral space that binds itself as a tristimulus one. "
		"A color space can be either spectral or tristimulus but not both.");

private:
	using Base = TColorSpaceDefinitionHelper<COLOR_SPACE, REF_WHITE>;

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

}// end namespace ph::math

#include "Math/Color/color_spaces.ipp"
