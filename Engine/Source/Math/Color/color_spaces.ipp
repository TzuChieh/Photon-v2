#pragma once

#include "Math/Color/color_spaces.h"
#include "Math/TMatrix3.h"
#include "Common/assertion.h"

namespace ph::math
{
	
template<>
class TColorSpaceDefinition<EColorSpace::CIE_XYZ> final : 
	public TTristimulusColorSpaceDefinitionHelper<EColorSpace::CIE_XYZ, EReferenceWhite::D65>
{
public:
	inline static TristimulusValues toCIEXYZ(const TristimulusValues& thisColor)
	{
		// Already in CIE-XYZ space, simply return the source color.
		return thisColor;
	}

	inline static TristimulusValues fromCIEXYZ(const TristimulusValues& CIEXYZColor)
	{
		// Already in CIE-XYZ space, simply return the source color.
		return CIEXYZColor;
	}
};

/*! @brief Linear sRGB color space definitions.

Using D65 as reference white point (standard sRGB is based on D65).

Good reference on how to derive the conversion matrix mathematically:
[1] http://www.ryanjuckett.com/programming/rgb-color-space-conversion/?start=2

A wide collection of color space related data, code, math... etc.
[2] http://www.brucelindbloom.com/index.html

Other references:
[3] http://www.color.org/sRGB.xalter
[4] https://en.wikipedia.org/wiki/SRGB
*/
template<>
class TColorSpaceDefinition<EColorSpace::Linear_sRGB> final :
	public TTristimulusColorSpaceDefinitionHelper<EColorSpace::Linear_sRGB, EReferenceWhite::D65>
{
public:
	/*! @brief Converts linear sRGB color to CIE XYZ color.

	Note that we did NOT use the matrices listed in Bruce's site [2] (http://www.brucelindbloom.com/index.html?ReferenceImages.html).
	The reason is that it seems to have some numeric precision issues in the calculation of the 
	conversion matrices, as they are different from the ones in Ryan's site [1] and Wiki page [4]. 
	Also note that many other sources have calculated matrices that are different between each 
	other's results.

	An informative thread discussing this issue: https://stackoverflow.com/questions/66360637/which-matrix-is-correct-to-map-xyz-to-linear-rgb-for-srgb

	So how do we transform sRGB colors? To my best understanding, Ryan's calculation is correct, 
	and we use the matrices listed in a W3C definition in CSS Color 4: https://drafts.csswg.org/css-color-4/#color-conversion-code, 
	which is the highest precision matrices (and consistent with Ryan's result [1]) that I can 
	find on the Internet.
	*/
	inline static TristimulusValues toCIEXYZ(const TristimulusValues& thisColor)
	{
		using Matrix = TMatrix3<ColorValue>;

		const Matrix M(Matrix::Elements{{
			{0.41239079926595934, 0.357584339383878,   0.1804807884018343},
			{0.21263900587151027, 0.715168678767756,   0.07219231536073371},
			{0.01933081871559182, 0.11919477979462598, 0.9505321522496607}
		}});

		return M.multiplyVector(thisColor);
	}

	inline static TristimulusValues fromCIEXYZ(const TristimulusValues& CIEXYZColor)
	{
		using Matrix = TMatrix3<ColorValue>;

		const Matrix M(Matrix::Elements{{
			{ 3.2409699419045226,  -1.537383177570094,  -0.4986107602930034},
			{-0.9692436362808796,   1.8759675015077202,  0.04155505740717559},
			{ 0.05563007969699366, -0.20397695888897652, 1.0569715142428786}
		}});

		return M.multiplyVector(CIEXYZColor);
	}
};

template<>
class TColorSpaceDefinition<EColorSpace::sRGB> final :
	public TTristimulusColorSpaceDefinitionHelper<EColorSpace::Linear_sRGB, EReferenceWhite::D65>
{
public:
	/*! @brief Forward gamma correction on single color component (linear-sRGB to sRGB).
	*/
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

	/*! @brief Forward gamma correction (linear-sRGB to sRGB).
	*/
	inline static TristimulusValues gammaCompress(const TristimulusValues& linearSRGBColor)
	{
		return
		{
			gammaCompressSingleChannel(linearSRGBColor[0]), 
			gammaCompressSingleChannel(linearSRGBColor[1]), 
			gammaCompressSingleChannel(linearSRGBColor[2])
		};
	}

	/*! @brief Inverse gamma correction (sRGB to linear-sRGB).
	*/
	inline static TristimulusValues gammaExpand(const TristimulusValues& nonlinearSRGBColor)
	{
		return
		{
			gammaExpandSingleChannel(nonlinearSRGBColor[0]),
			gammaExpandSingleChannel(nonlinearSRGBColor[1]),
			gammaExpandSingleChannel(nonlinearSRGBColor[2])
		};
	}

	inline static TristimulusValues toCIEXYZ(const TristimulusValues& thisColor)
	{
		const TristimulusValues linearSRGBColor = gammaExpand(thisColor);
		return TColorSpaceDefinition<EColorSpace::Linear_sRGB>::toCIEXYZ(linearSRGBColor);
	}

	inline static TristimulusValues fromCIEXYZ(const TristimulusValues& CIEXYZColor)
	{
		const TristimulusValues linearSRGBColor = TColorSpaceDefinition<EColorSpace::Linear_sRGB>::fromCIEXYZ(CIEXYZColor);
		return gammaCompress(linearSRGBColor);
	}
};

}// end namespace ph::math
