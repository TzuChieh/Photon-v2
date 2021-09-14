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

template<>
class TColorSpaceDefinition<EColorSpace::Linear_sRGB> final :
	public TTristimulusColorSpaceDefinitionHelper<EColorSpace::Linear_sRGB, EReferenceWhite::D65>
{
public:
	inline static TristimulusValues toCIEXYZ(const TristimulusValues& thisColor)
	{
		using Matrix = TMatrix3<ColorValue>;

		const Matrix M(Matrix::Elements{{
			{0.4124564, 0.3575761, 0.1804375},
			{0.2126729, 0.7151522, 0.0721750},
			{0.0193339, 0.1191920, 0.9503041}
		}});

		return M.multiplyVector(thisColor);
	}

	inline static TristimulusValues fromCIEXYZ(const TristimulusValues& CIEXYZColor)
	{
		using Matrix = TMatrix3<ColorValue>;

		const Matrix M(Matrix::Elements{{
			{ 3.2404542, -1.5371385, -0.4985314},
			{-0.9692660,  1.8760108,  0.0415560},
			{ 0.0556434, -0.2040259,  1.0572252}
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
