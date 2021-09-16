#pragma once

#include "Math/Color/color_basics.h"
#include "Utility/IUninstantiable.h"

#include <concepts>

namespace ph::math
{

template
<
	typename        DefType,
	EReferenceWhite SRC_REF_WHITE,
	EReferenceWhite DST_REF_WHITE,
	typename        T = ColorValue
>
concept CChromaticAdaptationDefinition = requires (TTristimulusValues<T> CIEXYZColor)
{
	{ DefType::getAlgorithm() } noexcept -> std::same_as<EChromaticAdaptation>;
	{ DefType::template canAdapt<SRC_REF_WHITE, DST_REF_WHITE>() } noexcept -> std::same_as<bool>;
	{ DefType::template adapt<SRC_REF_WHITE, DST_REF_WHITE>(CIEXYZColor) } -> std::same_as<TTristimulusValues<T>>;
};

template<EChromaticAdaptation ALGORITHM>
class TChromaticAdaptationDefinitionHelper : private IUninstantiable
{
	static_assert(ALGORITHM != EChromaticAdaptation::UNSPECIFIED);

public:
	inline static constexpr EChromaticAdaptation getAlgorithm() noexcept
	{
		return ALGORITHM;
	}
};

/*! @brief Sinkhole for undefined chromatic adaptation routines.
Specialize the class to provide definitions for the specified adaptation configuration. 
Must satisfy CChromaticAdaptationDefinition.
*/
template<EChromaticAdaptation ALGORITHM>
class TChromaticAdaptationDefinition final
{};

// Unspecified adaption configuration must not be a valid definition.
static_assert(!CChromaticAdaptationDefinition<TChromaticAdaptationDefinition<
	EChromaticAdaptation::UNSPECIFIED, EReferenceWhite::UNSPECIFIED, EReferenceWhite::UNSPECIFIED>>);

}// end namespace ph::math

#include "Math/Color/chromatic_adaptations.ipp"
