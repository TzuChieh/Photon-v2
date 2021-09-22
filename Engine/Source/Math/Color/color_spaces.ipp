#pragma once

#include "Math/Color/color_spaces.h"
#include "Math/TMatrix3.h"
#include "Math/Color/spectral_samples.h"
#include "Utility/IUninstantiable.h"
#include "Math/math_exceptions.h"
#include "Common/assertion.h"
#include "Math/TArithmeticArray.h"

#include <cmath>

namespace ph::math
{
	
template<EColorSpace COLOR_SPACE, EReferenceWhite REF_WHITE>
class TColorSpaceDefinitionHelper : private IUninstantiable
{
protected:
	inline static consteval EColorSpace getColorSpace() noexcept
	{
		return COLOR_SPACE;
	}
};

template<EColorSpace COLOR_SPACE, EReferenceWhite REF_WHITE>
class TTristimulusColorSpaceDefinitionHelper : private IUninstantiable
{
public:
	inline static consteval EColorSpace getColorSpace() noexcept
	{
		return COLOR_SPACE;
	}

	inline static consteval bool isTristimulus() noexcept
	{
		return true;
	}

	inline static consteval EReferenceWhite getReferenceWhite() noexcept
	{
		return REF_WHITE;
	}
};

template<EColorSpace COLOR_SPACE, EColorSpace BOUND_TRISTIMULUS_COLOR_SPACE>
class TSpectralColorSpaceDefinitionHelper : private IUninstantiable
{
	static_assert(COLOR_SPACE != BOUND_TRISTIMULUS_COLOR_SPACE,
		"Cannot define a spectral space that binds itself as a tristimulus one. "
		"A color space can be either spectral or tristimulus but not both.");

public:
	inline static consteval EColorSpace getColorSpace() noexcept
	{
		return COLOR_SPACE;
	}

	inline static consteval bool isTristimulus() noexcept
	{
		return false;
	}

	inline static consteval EColorSpace getBoundTristimulusColorSpace() noexcept
	{
		return BOUND_TRISTIMULUS_COLOR_SPACE;
	}
};

template<typename T>
class TColorSpaceDefinition<EColorSpace::CIE_XYZ, T> final : 
	public TTristimulusColorSpaceDefinitionHelper<EColorSpace::CIE_XYZ, EReferenceWhite::D65>
{
public:
	inline static TTristimulusValues<T> toCIEXYZ(const TTristimulusValues<T>& thisColor)
	{
		// Already in CIE-XYZ space, simply return the source color.
		return thisColor;
	}

	inline static TTristimulusValues<T> fromCIEXYZ(const TTristimulusValues<T>& CIEXYZColor)
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
template<typename T>
class TColorSpaceDefinition<EColorSpace::Linear_sRGB, T> final :
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
	inline static TTristimulusValues<T> toCIEXYZ(const TTristimulusValues<T>& thisColor)
	{
		const TMatrix3<T> M(
			0.41239079926595934, 0.357584339383878,   0.1804807884018343,
			0.21263900587151027, 0.715168678767756,   0.07219231536073371,
			0.01933081871559182, 0.11919477979462598, 0.9505321522496607);

		return M.multiplyVector(thisColor);
	}

	inline static TTristimulusValues<T> fromCIEXYZ(const TTristimulusValues<T>& CIEXYZColor)
	{
		const TMatrix3<T> M(
			 3.2409699419045226,  -1.537383177570094,  -0.4986107602930034,
			-0.9692436362808796,   1.8759675015077202,  0.04155505740717559,
			 0.05563007969699366, -0.20397695888897652, 1.0569715142428786);

		return M.multiplyVector(CIEXYZColor);
	}
};

template<typename T>
class TColorSpaceDefinition<EColorSpace::sRGB, T> final :
	public TTristimulusColorSpaceDefinitionHelper<EColorSpace::sRGB, EReferenceWhite::D65>
{
public:
	/*! @brief Forward gamma correction on single color component (linear-sRGB to sRGB).
	*/
	inline static T gammaCompressSingleChannel(const T linearColorValue)
	{
		PH_ASSERT_GE(linearColorValue, 0.0);
		PH_ASSERT_LE(linearColorValue, 1.0);

		if(linearColorValue <= 0.0031308)
		{
			return static_cast<T>(12.92 * linearColorValue);
		}

		return static_cast<T>(1.055 * std::pow(linearColorValue, 1.0 / 2.4) - 0.055);
	}

	/*! @brief Inverse gamma correction on single color component (sRGB to linear-sRGB).
	*/
	inline static T gammaExpandSingleChannel(const T nonlinearColorValue)
	{
		PH_ASSERT_GE(nonlinearColorValue, 0.0);
		PH_ASSERT_LE(nonlinearColorValue, 1.0);

		if(nonlinearColorValue <= 0.04045)
		{
			return static_cast<T>(nonlinearColorValue / 12.92);
		}

		return static_cast<T>(std::pow((nonlinearColorValue + 0.055) / 1.055, 2.4));
	}

	/*! @brief Forward gamma correction (linear-sRGB to sRGB).
	*/
	inline static TTristimulusValues<T> gammaCompress(const TTristimulusValues<T>& linearSRGBColor)
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
	inline static TTristimulusValues<T> gammaExpand(const TTristimulusValues<T>& nonlinearSRGBColor)
	{
		return
		{
			gammaExpandSingleChannel(nonlinearSRGBColor[0]),
			gammaExpandSingleChannel(nonlinearSRGBColor[1]),
			gammaExpandSingleChannel(nonlinearSRGBColor[2])
		};
	}

	inline static TTristimulusValues<T> toCIEXYZ(const TTristimulusValues<T>& thisColor)
	{
		const TTristimulusValues<T> linearSRGBColor = gammaExpand(thisColor);
		return TColorSpaceDefinition<EColorSpace::Linear_sRGB, T>::toCIEXYZ(linearSRGBColor);
	}

	inline static TTristimulusValues<T> fromCIEXYZ(const TTristimulusValues<T>& CIEXYZColor)
	{
		const TTristimulusValues<T> linearSRGBColor = TColorSpaceDefinition<EColorSpace::Linear_sRGB, T>::fromCIEXYZ(CIEXYZColor);
		return gammaCompress(linearSRGBColor);
	}
};

template<typename T>
class TColorSpaceDefinition<EColorSpace::CIE_xyY, T> final :
	public TTristimulusColorSpaceDefinitionHelper<EColorSpace::CIE_xyY, EReferenceWhite::D65>
{
public:
	/*!
	Normally, the Y component in xyY should be [0, 1]; we allow Y > 1 to support HDR.
	Reference: http://www.brucelindbloom.com/index.html?Eqn_xyY_to_XYZ.html
	*/
	inline static TTristimulusValues<T> toCIEXYZ(const TTristimulusValues<T>& thisColor)
	{
		// Negative luminance does not make sense (sucking light out of your eyeballs? hmmm), so we clamp to 0
		const T x = thisColor[0];
		const T y = thisColor[1];
		const T Y = std::max(thisColor[2], static_cast<T>(0));

		if(y != static_cast<T>(0))
		{
			const T Y_over_y = Y / y;
			return
			{
				Y_over_y * x,
				Y, 
				Y_over_y * (static_cast<T>(1) - x - y)
			};
		}
		else
		{
			// All zero for y = 0;
			// the Colour python library returns (y, y, y) in this case (equivalent to our implementation)
			return {0.0, 0.0, 0.0};
		}
	}

	/*!
	Reference: http://www.brucelindbloom.com/index.html?Eqn_XYZ_to_xyY.html
	*/
	inline static TTristimulusValues<T> fromCIEXYZ(const TTristimulusValues<T>& CIEXYZColor)
	{
		// CIE XYZ color are defined to be non-negative, clamp negative values to 0
		const T CIE_X = std::max(CIEXYZColor[0], static_cast<T>(0));
		const T CIE_Y = std::max(CIEXYZColor[1], static_cast<T>(0));
		const T CIE_Z = std::max(CIEXYZColor[2], static_cast<T>(0));

		const T sumXYZ = CIE_X + CIE_Y + CIE_Z;
		PH_ASSERT_GE(sumXYZ, static_cast<T>(0));

		if(sumXYZ > static_cast<T>(0))
		{
			const T rcpSumXYZ = static_cast<T>(1) / sumXYZ;

			return 
			{
				CIE_X * rcpSumXYZ,// x
				CIE_Y * rcpSumXYZ,// y
				CIE_Y             // Y (same as CIE XYZ's Y)
			};
		}
		else
		{
			// So the input XYZ color is black (X = Y = Z = 0)
			// In this case, we should return our reference white's chromaticity coordinates (xyY's xy),
			const auto refWhiteChromaticity = chromaticity_of<T>(getReferenceWhite());

			return
			{
				refWhiteChromaticity[0],
				refWhiteChromaticity[1],
				static_cast<T>(0)       // Y = 0 as it is black
			};
		}
	}
};

/*!
Note that the white point of ACEScg is the same as ACES2065-1 [2] as stated in the documentation [3].

References:
[1] General Documentation Page
https://www.oscars.org/science-technology/aces/aces-documentation
[2] Informative Notes on SMPTE ST 2065-1 ¡V Academy Color Encoding Specification (ACES)
http://j.mp/TB-2014-004
[3] ACEScg ¡X A Working Space for CGI Render and Compositing
http://j.mp/S-2014-004
[4] AMPAS ACES project repo: readme for matrix transformations
https://github.com/ampas/aces-dev/blob/dev/transforms/ctl/README-MATRIX.md
[5] The Colour python library source code
https://github.com/colour-science/colour/blob/develop/colour/colorimetry/datasets/illuminants/chromaticity_coordinates.py
[6] Nice overview of ACES color space
https://chrisbrejon.com/cg-cinematography/chapter-1-5-academy-color-encoding-system-aces/
*/
template<typename T>
class TColorSpaceDefinition<EColorSpace::ACEScg, T> final :
	public TTristimulusColorSpaceDefinitionHelper<EColorSpace::ACEScg, EReferenceWhite::ACES>
{
public:
	inline static TTristimulusValues<T> toCIEXYZ(const TTristimulusValues<T>& thisColor)
	{
		// According to [4], this is the matrix for transformation from ACEScg (AP1) to CIE-XYZ
		const TMatrix3<T> M(
			 0.6624541811, 0.1340042065, 0.1561876870,
			 0.2722287168, 0.6740817658, 0.0536895174,
			-0.0055746495, 0.0040607335, 1.0103391003);

		return M.multiplyVector(thisColor);
	}

	inline static TTristimulusValues<T> fromCIEXYZ(const TTristimulusValues<T>& CIEXYZColor)
	{
		// According to [4], this is the matrix for transformation from CIE-XYZ to ACEScg (AP1)
		const TMatrix3<T> M(
			 1.6410233797, -0.3248032942, -0.2364246952,
			-0.6636628587,  1.6153315917,  0.0167563477,
			 0.0117218943, -0.0082844420,  0.9883948585);

		return M.multiplyVector(CIEXYZColor);
	}
};

namespace detail
{

template<typename T, CSpectralSampleProps SampleProps>
struct TSmitsSPDBasis final
{
	TArithmeticArray<T, SampleProps::NUM_SAMPLES> white;
	TArithmeticArray<T, SampleProps::NUM_SAMPLES> cyan;
	TArithmeticArray<T, SampleProps::NUM_SAMPLES> magenta;
	TArithmeticArray<T, SampleProps::NUM_SAMPLES> yellow;
	TArithmeticArray<T, SampleProps::NUM_SAMPLES> red;
	TArithmeticArray<T, SampleProps::NUM_SAMPLES> green;
	TArithmeticArray<T, SampleProps::NUM_SAMPLES> blue;

	inline TSmitsSPDBasis()
	{
		// Construct sampled SPD bases for Smits' algorithm.
	
		using SmitsSPDValueType = spectral_data::ArraySmits::value_type;
		const std::size_t NUM_SMITS_POINTS = std::tuple_size_v<spectral_data::ArraySmits>;

		const auto sampledValuesWhite = make_piecewise_avg_spectral_samples<T, SmitsSPDValueType, SampleProps>(
			spectral_data::smits_linear_sRGB_to_spectrum_E_wavelengths_nm().data(),
			spectral_data::smits_linear_sRGB_to_spectrum_E_white().data(),
			NUM_SMITS_POINTS);

		const auto sampledValuesCyan = make_piecewise_avg_spectral_samples<T, SmitsSPDValueType, SampleProps>(
			spectral_data::smits_linear_sRGB_to_spectrum_E_wavelengths_nm().data(),
			spectral_data::smits_linear_sRGB_to_spectrum_E_cyan().data(),
			NUM_SMITS_POINTS);

		const auto sampledValuesMagenta = make_piecewise_avg_spectral_samples<T, SmitsSPDValueType, SampleProps>(
			spectral_data::smits_linear_sRGB_to_spectrum_E_wavelengths_nm().data(),
			spectral_data::smits_linear_sRGB_to_spectrum_E_magenta().data(),
			NUM_SMITS_POINTS);

		const auto sampledValuesYellow = make_piecewise_avg_spectral_samples<T, SmitsSPDValueType, SampleProps>(
			spectral_data::smits_linear_sRGB_to_spectrum_E_wavelengths_nm().data(),
			spectral_data::smits_linear_sRGB_to_spectrum_E_yellow().data(),
			NUM_SMITS_POINTS);

		const auto sampledValuesRed = make_piecewise_avg_spectral_samples<T, SmitsSPDValueType, SampleProps>(
			spectral_data::smits_linear_sRGB_to_spectrum_E_wavelengths_nm().data(),
			spectral_data::smits_linear_sRGB_to_spectrum_E_red().data(),
			NUM_SMITS_POINTS);

		const auto sampledValuesGreen = make_piecewise_avg_spectral_samples<T, SmitsSPDValueType, SampleProps>(
			spectral_data::smits_linear_sRGB_to_spectrum_E_wavelengths_nm().data(),
			spectral_data::smits_linear_sRGB_to_spectrum_E_green().data(),
			NUM_SMITS_POINTS);

		const auto sampledValuesBlue = make_piecewise_avg_spectral_samples<T, SmitsSPDValueType, SampleProps>(
			spectral_data::smits_linear_sRGB_to_spectrum_E_wavelengths_nm().data(),
			spectral_data::smits_linear_sRGB_to_spectrum_E_blue().data(),
			NUM_SMITS_POINTS);

		white.set(sampledValuesWhite);
		cyan.set(sampledValuesCyan);
		magenta.set(sampledValuesMagenta);
		yellow.set(sampledValuesYellow);
		red.set(sampledValuesRed);
		green.set(sampledValuesGreen);
		blue.set(sampledValuesBlue);
	}
};

}// end namespace detail

template<typename T>
class TColorSpaceDefinition<EColorSpace::Spectral_Smits, T> final :
	public TSpectralColorSpaceDefinitionHelper<EColorSpace::Spectral_Smits, EColorSpace::Linear_sRGB>
{
public:
	inline static TSpectralSampleValues<T> upSample(const TTristimulusValues<T>& boundColor, const EColorUsage usage)
	{
		static const detail::TSmitsSPDBasis<T, DefaultSpectralSampleProps> basis;

		const T r = boundColor[0];
		const T g = boundColor[1];
		const T b = boundColor[2];

		TArithmeticArray<T, DefaultSpectralSampleProps::NUM_SAMPLES> spd(0);

		// The following steps mix in primary colors only as needed. Also, 
		// (r, g, b) = (1, 1, 1) will be mapped to a constant SPD with 
		// magnitudes = 1.

		// When R is minimum
		if(r <= g && r <= b)
		{
			spd.addLocal(basis.white * r);
			if(g <= b)
			{
				spd.addLocal(basis.cyan * (g - r));
				spd.addLocal(basis.blue * (b - g));
			}
			else
			{
				spd.addLocal(basis.cyan * (b - r));
				spd.addLocal(basis.green * (g - b));
			}
		}
		// When G is minimum
		else if(g <= r && g <= b)
		{
			spd.addLocal(basis.white * g);
			if(r <= b)
			{
				spd.addLocal(basis.magenta * (r - g));
				spd.addLocal(basis.blue * (b - r));
			}
			else
			{
				spd.addLocal(basis.magenta * (b - g));
				spd.addLocal(basis.red * (r - b));
			}
		}
		// When B is minimum
		else
		{
			spd.addLocal(basis.white * b);
			if(r <= g)
			{
				spd.addLocal(basis.yellow * (r - b));
				spd.addLocal(basis.green * (g - r));
			}
			else
			{
				spd.addLocal(basis.yellow * (g - b));
				spd.addLocal(basis.red * (r - g));
			}
		}
	
		// For things such as illuminants, scale its SPD so that constant SPDs matches D65.
		if constexpr(usage == EColorUsage::EMR)
		{
			spd.mulLocal(SPD_D65);
		}

		// For things such as reflectances, make sure energy conservation requirements are met.
		if constexpr(usage == EColorUsage::ECF)
		{
			spd.clampLocal(0, 1);
		}

		return spd.toArray();
	}

	inline static TTristimulusValues<T> downSample(const TSpectralSampleValues<T>& sampleValues, const EColorUsage usage)
	{
		// TODO
	}
};

// End Color Space Definitions

// Unspecified color space must not be a valid color space.
static_assert(!CColorSpaceDefinition<TColorSpaceDefinition<EColorSpace::UNSPECIFIED, float>, float>);
static_assert(!CColorSpaceDefinition<TColorSpaceDefinition<EColorSpace::UNSPECIFIED, double>, double>);

template<EColorSpace SRC_COLOR_SPACE, EColorSpace DST_COLOR_SPACE, typename T, EChromaticAdaptation ALGORITHM = EChromaticAdaptation::Bradford>
inline decltype(auto) transform_color(const auto& srcColorValues, const EColorUsage usage)
{
	using SrcColorSpaceDef = TColorSpaceDefinition<SRC_COLOR_SPACE, T>;
	using DstColorSpaceDef = TColorSpaceDefinition<DST_COLOR_SPACE, T>;

	// Sanity checks

	static_assert(CColorSpaceDefinition<SrcColorSpaceDef>,
		"Source color space has no corresponding definition.");

	static_assert(CColorSpaceDefinition<DstColorSpaceDef>,
		"Destination color space has no corresponding definition.");

	// Type of source color values must match the category of its color space
	{
		using SrcColorValues = std::remove_cvref_t<decltype(srcColorValues)>;
		if constexpr(SrcColorSpaceDef::isTristimulus())
		{
			static_assert(std::is_same_v<SrcColorValues, TTristimulusValues<T>>);
		}
		else
		{
			static_assert(std::is_same_v<SrcColorValues, TSpectralSampleValues<T>>);
		}
	}

	// There are 4 possible conversion scenarios, we process them case by case:
	//                       
	//               To  +-----------------+-----------------+
	//  From             | tristimulus (T) | spectral    (S) |
	// +-----------------+-----------------+-----------------+
	// | tristimulus (T) |     T --> T     |     T --> S     |
	// +-----------------+-----------------+-----------------+
	// | spectral    (S) |     S --> T     |     S --> S     |
	// +-----------------+-----------------+-----------------+
	//

	// Case 1: S --> S, down sample input spectral samples to tristimulus, converting it into a T --> S case.
	if constexpr(!SrcColorSpaceDef::isTristimulus() && !DstColorSpaceDef::isTristimulus())
	{
		if(usage == EColorUsage::UNSPECIFIED)
		{
			throw ColorError(
				"A color usage must be specified when converting spectral color samples.");
		}

		constexpr EColorSpace DOWN_SAMPLE_COLOR_SPACE = SrcColorSpaceDef::getBoundTristimulusColorSpace();
		const TTristimulusValues<T> downSampledValues = SrcColorSpaceDef::downSample(srcColorValues, usage);
		
		return transform_color<DOWN_SAMPLE_COLOR_SPACE, DST_COLOR_SPACE, T, ALGORITHM>(
			downSampledValues, usage);
	}

	// Case 2: T --> S, convert input tristimulus values to dst's bound space first (a T --> T case),
	//         then perform an up sample.
	if constexpr(SrcColorSpaceDef::isTristimulus() && !DstColorSpaceDef::isTristimulus())
	{
		constexpr EColorSpace BOUND_COLOR_SPACE = DstColorSpaceDef::getBoundTristimulusColorSpace();
		
		const TTristimulusValues<T> boundColor = transform_color<SRC_COLOR_SPACE, BOUND_COLOR_SPACE, T, ALGORITHM>(
			srcColorValues, usage);
	
		return DstColorSpaceDef::upSample(boundColor, usage);
	}

	// Case 3: S --> T, down sample input spectral samples to src's bound space first, then convert
	//         the bound color to dst space (a T --> T case).
	if constexpr(!SrcColorSpaceDef::isTristimulus() && DstColorSpaceDef::isTristimulus())
	{
		const TTristimulusValues<T> boundColor = SrcColorSpaceDef::downSample(srcColorValues, usage);

		constexpr EColorSpace BOUND_COLOR_SPACE = SrcColorSpaceDef::getBoundTristimulusColorSpace();
		
		return transform_color<BOUND_COLOR_SPACE, DST_COLOR_SPACE, T, ALGORITHM>(
			boundColor, usage);
	}

	// Case 4: T --> T, perform color conversion by using CIE XYZ as an intermediate space
	if constexpr(SrcColorSpaceDef::isTristimulus() && DstColorSpaceDef::isTristimulus())
	{
		// No conversion needed if they are in the same color space
		if constexpr(SRC_COLOR_SPACE == DST_COLOR_SPACE)
		{
			return srcColorValues;
		}

		// Convert to CIE XYZ first
		TTristimulusValues<T> srcCIEXYZColor = SrcColorSpaceDef::toCIEXYZ(srcColorValues);

		// Perform chromatic adaptation if needed
		if constexpr(SrcColorSpaceDef::getReferenceWhite() != DstColorSpaceDef::getReferenceWhite())
		{
			srcCIEXYZColor = chromatic_adapt<ALGORITHM, T>(
				srcCIEXYZColor,
				SrcColorSpaceDef::getReferenceWhite(),
				DstColorSpaceDef::getReferenceWhite());
		}

		// Finally, convert to the destination color space
		return DstColorSpaceDef::fromCIEXYZ(srcCIEXYZColor);
	}

	PH_ASSERT_UNREACHABLE_SECTION();
	return TTristimulusValues<T>{0, 0, 0};
}

}// end namespace ph::math
