#pragma once

#include "Math/Color/color_basics.h"
#include "Utility/IUninstantiable.h"

#include <concepts>

namespace ph::math
{

template<typename T>
concept CChromaticAdaptionDefinition = requires (TristimulusValues CIEXYZColor)
{
	{ T::getAlgorithm() } noexcept -> std::same_as<EChromaticAdaption>;
	{ T::getReferenceWhite1() } noexcept -> std::same_as<EReferenceWhite>;
	{ T::getReferenceWhite2() } noexcept -> std::same_as<EReferenceWhite>;
	{ T::from1To2(CIEXYZColor) } -> std::same_as<TristimulusValues>;
	{ T::from2To1(CIEXYZColor) } -> std::same_as<TristimulusValues>;
};

template
<
	EChromaticAdaption ALGORITHM, 
	EReferenceWhite    REFERENCE_WHITE_1, 
	EReferenceWhite    REFERENCE_WHITE_2
>
class TChromaticAdaptionDefinitionHelper : private IUninstantiable
{
	static_assert(ALGORITHM != EChromaticAdaption::UNSPECIFIED);

	static_assert(REFERENCE_WHITE_1 != REFERENCE_WHITE_2,
		"Same reference whites provided for a chromatic adaption definition. "
		"As a result, there is nothing to adapt.");

public:
	inline static constexpr EChromaticAdaption getAlgorithm() noexcept
	{
		return ALGORITHM;
	}

	inline static constexpr EColorSpace getReferenceWhite1() noexcept
	{
		return REFERENCE_WHITE_1;
	}

	inline static constexpr EColorSpace getReferenceWhite2() noexcept
	{
		return REFERENCE_WHITE_2;
	}
};

/*! @brief Sinkhole for undefined chromatic adaption routines.
Specialize the class to provide definitions for the specified adaption configuration. 
Must satisfy CChromaticAdaptionDefinition.
*/
template
<
	EChromaticAdaption ALGORITHM,
	EReferenceWhite    REFERENCE_WHITE_1,
	EReferenceWhite    REFERENCE_WHITE_2
>
class TChromaticAdaptionDefinition final
{};

// Unspecified adaption configuration must not be a valid definition.
static_assert(!CChromaticAdaptionDefinition<TChromaticAdaptionDefinition<
	EChromaticAdaption::UNSPECIFIED, EReferenceWhite::UNSPECIFIED, EReferenceWhite::UNSPECIFIED>>);

}// end namespace ph::math

#include "Math/Color/chromatic_adaptions.ipp"
