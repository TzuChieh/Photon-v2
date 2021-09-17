#pragma once

#include "Math/Color/chromatic_adaptations.h"
#include "Common/assertion.h"

namespace ph::math
{

/*!
References: 
[1] http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
[2] https://cran.r-project.org/web/packages/spacesXYZ/vignettes/adaptation.html
[3] https://en.wikipedia.org/wiki/LMS_color_space
*/
template<typename Derived, EChromaticAdaptation ALGORITHM, typename T>
inline auto TVonKriesBasedLinearCATDefinitionHelper<Derived, ALGORITHM, T>::adapt(
	const TTristimulusValues<T>& CIEXYZColor,
	const EReferenceWhite        srcRefWhite,
	const EReferenceWhite        dstRefWhite)
-> TTristimulusValues<T>
{
	static_assert(CVonKriesBasedLinearCATImpl<Derived, T>,
		"Derived CAT definition class does not implement required methods.");

	PH_ASSERT(srcRefWhite != EReferenceWhite::UNSPECIFIED);
	PH_ASSERT(dstRefWhite != EReferenceWhite::UNSPECIFIED);
	PH_ASSERT(srcRefWhite != dstRefWhite);

	const TMatrix3<T> CIEXYZToConeResponse = Derived::getCIEXYZToConeResponse();
	const TMatrix3<T> coneResponseToCIEXYZ = Derived::getConeResponseToCIEXYZ();

	const TTristimulusValues<T> srcConeResponse = CIEXYZToConeResponse.multiplyVector(CIEXYZ_of<T>(srcRefWhite));
	const TTristimulusValues<T> dstConeResponse = CIEXYZToConeResponse.multiplyVector(CIEXYZ_of<T>(dstRefWhite));

	PH_ASSERT_NE(srcConeResponse[0], 0);
	PH_ASSERT_NE(srcConeResponse[1], 0);
	PH_ASSERT_NE(srcConeResponse[2], 0);

	const TTristimulusValues<T> coneResponseScale = {
		dstConeResponse[0] / srcConeResponse[0],
		dstConeResponse[1] / srcConeResponse[1],
		dstConeResponse[2] / srcConeResponse[2]
	};
	
	const auto coneResponseScaleMat = TMatrix3<T>().setScale(coneResponseScale);
	const auto CATMatrix = coneResponseToCIEXYZ.mul(coneResponseScale).mul(CIEXYZToConeResponse);
	return CATMatrix.multiplyVector(CIEXYZColor);
}

/*! @brief Adapts by simply scale by the corresponding white points--identity CAT matrices are used.
*/
template<typename T>
class TChromaticAdaptationDefinition<EChromaticAdaptation::XYZ_Scaling, T> final
	: public TVonKriesBasedLinearCATDefinitionHelper<
		TChromaticAdaptationDefinition<EChromaticAdaptation::Bradford, T>,
		EChromaticAdaptation::Bradford,
		T>
{
public:
	inline static TMatrix3<T> getCIEXYZToConeResponse()
	{
		return TMatrix3<T>().setIdentity();
	}

	inline static TMatrix3<T> getConeResponseToCIEXYZ()
	{
		return TMatrix3<T>().setIdentity();
	}
};

/*!
References:
[1] http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
[2] Dejana Đorđević et al., 
    Performance Of Five Chromatic Adaptation Transforms Using Large Number Of Color Patches,
    ACTA GRAPHICA, 2009
*/
template<typename T>
class TChromaticAdaptationDefinition<EChromaticAdaptation::Bradford, T> final
	: public TVonKriesBasedLinearCATDefinitionHelper<
		TChromaticAdaptationDefinition<EChromaticAdaptation::Bradford, T>,
		EChromaticAdaptation::Bradford,
		T>
{
public:
	inline static TMatrix3<T> getCIEXYZToConeResponse()
	{
		return TMatrix3<T>(
			 0.8951000,  0.2664000, -0.1614000,
			-0.7502000,  1.7135000,  0.0367000,
			 0.0389000, -0.0685000,  1.0296000);
	}

	inline static TMatrix3<T> getConeResponseToCIEXYZ()
	{
		return TMatrix3<T>(
			 0.9869929054667121899,   -0.14705425642099010066,  0.15996265166373123948,
			 0.43230526972339451002,   0.51836027153677753834,  0.049291228212855612148,
			-0.0085286645751773312464, 0.040042821654084864313, 0.96848669578754998478);
	}
};

// Unspecified adaption configuration must not be a valid definition.
static_assert(!CChromaticAdaptationDefinition<
	TChromaticAdaptationDefinition<EChromaticAdaptation::UNSPECIFIED, ColorValue>, ColorValue>);

}// end namespace ph::math
