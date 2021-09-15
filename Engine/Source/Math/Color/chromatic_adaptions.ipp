#pragma once

#include "Math/Color/chromatic_adaptions.h"

namespace ph::math
{

template<>
class TChromaticAdaptionDefinition<
	EChromaticAdaption::Bradford,
	EReferenceWhite::D65,
	EReferenceWhite::D50> final :

	public TChromaticAdaptionDefinitionHelper<
		EChromaticAdaption::Bradford,
		EReferenceWhite::D65,
		EReferenceWhite::D50>
{
public:
	inline static ColorValue gammaCompressSingleChannel(const ColorValue linearColorValue)
	{
		PH_ASSERT_GE(linearColorValue, 0.0);
		PH_ASSERT_LE(linearColorValue, 1.0);

		if(linearColorValue <= 0.0031308)
		{
			return static_cast<ColorValue>(12.92 * linearColorValue);
		}

		return static_cast<ColorValue>(1.055 * std::pow(linearColorValue, 1.0 / 2.4) - 0.055);
	}

	/*! @brief Inverse gamma correction on single color component (sRGB to linear-sRGB).
	*/
	inline static ColorValue gammaExpandSingleChannel(const ColorValue nonlinearColorValue)
	{
		PH_ASSERT_GE(nonlinearColorValue, 0.0);
		PH_ASSERT_LE(nonlinearColorValue, 1.0);

		if(nonlinearColorValue <= 0.04045)
		{
			return static_cast<ColorValue>(nonlinearColorValue / 12.92);
		}

		return static_cast<ColorValue>(std::pow((nonlinearColorValue + 0.055) / 1.055, 2.4));
	}

}// end namespace ph::math
