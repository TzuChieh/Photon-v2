#pragma once

#include "Math/Color/color_spaces.h"
#include "Math/TMatrix3.h"

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

		// TODO: check
		const Matrix M(Matrix::Elements{{
			{0.4124564, 0.3575761, 0.1804375},
			{0.2126729, 0.7151522, 0.0721750},
			{0.0193339, 0.1191920, 0.9503041}
		}});

		return M.multiplyVector(thisColor);
	}

	inline static TristimulusValues fromCIEXYZ(const TristimulusValues& CIEXYZColor)
	{
		// TODO
		return CIEXYZColor;
	}
};

}// end namespace ph::math
