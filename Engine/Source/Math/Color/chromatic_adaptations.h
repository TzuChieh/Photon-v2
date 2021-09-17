#pragma once

#include "Math/Color/color_basics.h"
#include "Utility/IUninstantiable.h"
#include "Math/TMatrix3.h"

#include <concepts>

namespace ph::math
{

template<typename DefType, typename T>
concept CChromaticAdaptationDefinition = requires (
	EReferenceWhite       srcRefWhite,
	EReferenceWhite       dstRefWhite,
	TTristimulusValues<T> CIEXYZColor)
{
	{ DefType::getAlgorithm() } noexcept -> std::same_as<EChromaticAdaptation>;
	{ DefType::adapt(CIEXYZColor, srcRefWhite, dstRefWhite) } -> std::same_as<TTristimulusValues<T>>;
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

template<typename DefType, typename T>
concept CVonKriesBasedLinearCATImpl = requires ()
{
	{ DefType::getCIEXYZToConeResponse() } -> std::same_as<TMatrix3<T>>;
	{ DefType::getConeResponseToCIEXYZ() } -> std::same_as<TMatrix3<T>>;
};

template<typename Derived, EChromaticAdaptation ALGORITHM, typename T>
class TVonKriesBasedLinearCATDefinitionHelper : public TChromaticAdaptationDefinitionHelper<ALGORITHM>
{
public:
	static TTristimulusValues<T> adapt(
		const TTristimulusValues<T>& CIEXYZColor,
		const EReferenceWhite        srcRefWhite,
		const EReferenceWhite        dstRefWhite);
};

/*! @brief Sinkhole for undefined chromatic adaptation routines.
Specialize the class to provide definitions for the specified adaptation configuration. 
Must satisfy CChromaticAdaptationDefinition.
*/
template<EChromaticAdaptation ALGORITHM, typename T>
class TChromaticAdaptationDefinition final
{};

}// end namespace ph::math

#include "Math/Color/chromatic_adaptations.ipp"
