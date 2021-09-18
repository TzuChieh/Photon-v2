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
template<typename T>
inline TMatrix3<T> create_von_kries_linear_CAT_matrix(
	const TMatrix3<T>&    CIEXYZToConeResponse,
	const TMatrix3<T>&    ConeResponseToCIEXYZ,
	const EReferenceWhite srcRefWhite,
	const EReferenceWhite dstRefWhite)
{
	PH_ASSERT(srcRefWhite != EReferenceWhite::UNSPECIFIED);
	PH_ASSERT(dstRefWhite != EReferenceWhite::UNSPECIFIED);
	PH_ASSERT(srcRefWhite != dstRefWhite);

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
	return ConeResponseToCIEXYZ.mul(coneResponseScale).mul(CIEXYZToConeResponse);
}

/*! @brief Adapts by simply scale by the corresponding white points--identity CAT matrices are used.
*/
template<typename T>
class TChromaticAdaptationDefinition<EChromaticAdaptation::XYZScaling, T> final
	: public TChromaticAdaptationDefinitionHelper<EChromaticAdaptation::XYZScaling>
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

	inline static TTristimulusValues<T> adapt(
		const TTristimulusValues<T>& CIEXYZColor,
		const EReferenceWhite        srcRefWhite,
		const EReferenceWhite        dstRefWhite)
	{
		const auto CATMatrix = create_von_kries_linear_CAT_matrix<T>(
			getCIEXYZToConeResponse(),
			getConeResponseToCIEXYZ(),
			srcRefWhite,
			dstRefWhite);

		return CATMatrix.multiplyVector(CIEXYZColor);
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
	: public TChromaticAdaptationDefinitionHelper<EChromaticAdaptation::Bradford>
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

	inline static TTristimulusValues<T> adapt(
		const TTristimulusValues<T>& CIEXYZColor,
		const EReferenceWhite        srcRefWhite,
		const EReferenceWhite        dstRefWhite)
	{
		const auto CATMatrix = create_von_kries_linear_CAT_matrix<T>(
			getCIEXYZToConeResponse(),
			getConeResponseToCIEXYZ(),
			srcRefWhite,
			dstRefWhite);

		return CATMatrix.multiplyVector(CIEXYZColor);
	}
};

template<typename T>
class TChromaticAdaptationDefinition<EChromaticAdaptation::VonKries, T> final
	: public TChromaticAdaptationDefinitionHelper<EChromaticAdaptation::VonKries>
{
public:
	inline static TMatrix3<T> getCIEXYZToConeResponse()
	{
		return TMatrix3<T>(
			 0.4002400, 0.7076000, -0.0808100
			-0.2263000, 1.1653200,  0.0457000
			 0.0000000, 0.0000000,  0.9182200);
	}

	inline static TMatrix3<T> getConeResponseToCIEXYZ()
	{
		return TMatrix3<T>(
			1.8599363874558397422, -1.1293816185800914784,   0.21989740959619327624
			0.36119143624176752624, 0.63881246328504213303, -0.0000063705968386570599758
			0,                      0,                       1.0890636230968613186);
	}

	inline static TTristimulusValues<T> adapt(
		const TTristimulusValues<T>& CIEXYZColor,
		const EReferenceWhite        srcRefWhite,
		const EReferenceWhite        dstRefWhite)
	{
		const auto CATMatrix = create_von_kries_linear_CAT_matrix<T>(
			getCIEXYZToConeResponse(),
			getConeResponseToCIEXYZ(),
			srcRefWhite,
			dstRefWhite);

		return CATMatrix.multiplyVector(CIEXYZColor);
	}
};

// Unspecified adaption configuration must not be a valid definition.
static_assert(!CChromaticAdaptationDefinition<
	TChromaticAdaptationDefinition<EChromaticAdaptation::UNSPECIFIED, ColorValue>, ColorValue>);

}// end namespace ph::math
