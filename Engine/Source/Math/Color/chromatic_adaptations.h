#pragma once

#include "Math/Color/color_enums.h"
#include "Math/Color/color_basics.h"
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

/*! @brief Sinkhole for undefined chromatic adaptation routines.
Specialize the class to provide definitions for the specified adaptation configuration. 
Must satisfy CChromaticAdaptationDefinition.
*/
template<EChromaticAdaptation ALGORITHM, typename T>
class TChromaticAdaptationDefinition final
{
	// Available algorithms must provide definition and thus should not end up here.
	static_assert(ALGORITHM == EChromaticAdaptation::Unspecified,
		"No definition for the specified chromatic adaptation ALGORITHM.");
};

template<EChromaticAdaptation ALGORITHM, typename T>
TTristimulusValues<T> chromatic_adapt(
	const TTristimulusValues<T>& srcCIEXYZColor,
	EReferenceWhite              srcRefWhite,
	EReferenceWhite              dstRefWhite);

template<typename T>
TMatrix3<T> create_von_kries_linear_CAT_matrix(
	const TMatrix3<T>&    CIEXYZToConeResponse,
	const TMatrix3<T>&    ConeResponseToCIEXYZ,
	const EReferenceWhite srcRefWhite,
	const EReferenceWhite dstRefWhite);

template<typename T>
TMatrix3<T> create_von_kries_linear_CAT_matrix(
	const TMatrix3<T>&           CIEXYZToConeResponse,
	const TMatrix3<T>&           ConeResponseToCIEXYZ,
	const TTristimulusValues<T>& srcRefWhite,
	const TTristimulusValues<T>& dstRefWhite);

}// end namespace ph::math

#include "Math/Color/chromatic_adaptations.ipp"
