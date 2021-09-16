#pragma once

#include "Math/Color/chromatic_adaptations.h"

namespace ph::math
{

template<typename T>
inline TMatrix3<T> create_adaptation_matrix(
	const TMatrix3<T>&    CIEXYZToConeResponse,
	const TMatrix3<T>&    coneResponseToCIEXYZ,
	const EReferenceWhite srcRefWhite,
	const EReferenceWhite dstRefWhite)
{
	PH_ASSERT(srcRefWhite != EReferenceWhite::UNSPECIFIED);
	PH_ASSERT(dstRefWhite != EReferenceWhite::UNSPECIFIED);
	PH_ASSERT(srcRefWhite != dstRefWhite);

	const TTristimulusValues<T> srcConeResponse = CIEXYZToConeResponse.multiplyVector(CIEXYZ_of<T>(srcRefWhite));
	const TTristimulusValues<T> dstConeResponse = CIEXYZToConeResponse.multiplyVector(CIEXYZ_of<T>(dstRefWhite));

	// TOOD
}

template<typename T>
class TChromaticAdaptationDefinition<EChromaticAdaptation::Bradford, T> final
	: public TChromaticAdaptationDefinitionHelper<EChromaticAdaptation::Bradford>
{
public:
	inline static TTristimulusValues<T> adapt(
		const TTristimulusValues<T>& CIEXYZColor,
		const EReferenceWhite        srcRefWhite,
		const EReferenceWhite        dstRefWhite)
	{
		using Matrix = TMatrix3<ColorValue>;

		const Matrix M(Matrix::Elements{{
			{ 1.0479298208405488,   0.022946793341019088, -0.05019222954313557},
			{ 0.029627815688159344, 0.990434484573249,    -0.01707382502938514},
			{-0.009243058152591178, 0.015055144896577895,  0.7518742899580008}
		}});

		return M.multiplyVector(CIEXYZColorD65);
	}

	inline static TristimulusValues fromDstToSrc(const TristimulusValues& CIEXYZColorD50)
	{
		using Matrix = TMatrix3<ColorValue>;

		const Matrix M(Matrix::Elements{{
			{ 0.9554734527042182,   -0.023098536874261423, 0.0632593086610217},
			{-0.028369706963208136,  1.0099954580058226,   0.021041398966943008},
			{ 0.012314001688319899, -0.020507696433477912, 1.3303659366080753}
		}});

		return M.multiplyVector(CIEXYZColorD50);
	}
};

// Unspecified adaption configuration must not be a valid definition.
static_assert(!CChromaticAdaptationDefinition<
	TChromaticAdaptationDefinition<EChromaticAdaptation::UNSPECIFIED, ColorValue>, ColorValue>);

}// end namespace ph::math
