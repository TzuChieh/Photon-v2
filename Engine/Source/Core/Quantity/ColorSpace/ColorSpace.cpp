#include "Core/Quantity/ColorSpace.h"
#include "Common/assertion.h"
#include "Core/Quantity/ColorSpace/spectral_data.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/Quantity/SpectralData.h"

#include <array>
#include <iostream>

namespace ph
{

namespace
{

static SampledSpectralStrength kernel_X_D65;
static SampledSpectralStrength kernel_Y_D65;
static SampledSpectralStrength kernel_Z_D65;

static SampledSpectralStrength SPD_D65;

static SampledSpectralStrength SPD_Smits_E_white;
static SampledSpectralStrength SPD_Smits_E_cyan;
static SampledSpectralStrength SPD_Smits_E_magenta;
static SampledSpectralStrength SPD_Smits_E_yellow;
static SampledSpectralStrength SPD_Smits_E_red;
static SampledSpectralStrength SPD_Smits_E_green;
static SampledSpectralStrength SPD_Smits_E_blue;

}// end anonymous namespace

void ColorSpace::init()
{
	PH_ASSERT_MSG(!isInitialized(), "ColorSpace is already initialized");

	// Construct sampled D65 spectrum and normalize it by making its largest
	// value equals to 1.

	SPD_D65 = SpectralData::calcPiecewiseAveraged(
		spectral_data::CIE_D65_wavelengths_nm().data(),
		spectral_data::CIE_D65_values().data(),
		std::tuple_size<spectral_data::ArrayD65>::value);

	SPD_D65.divLocal(SPD_D65.max());

	// Sample XYZ color matching functions first, then later normalize it so 
	// that dotting them with sampled D65 spectrum is equivalent to computing
	// (X, Y, Z) tristimulus values and will yield (0.95047, 1, 1.08883).

	// sampling

	const std::size_t numXyzCmfPoints = std::tuple_size<spectral_data::ArrayXYZCMF>::value;

	kernel_X_D65 = SpectralData::calcPiecewiseAveraged(
		spectral_data::XYZ_CMF_CIE_1931_2_degree_wavelengths_nm().data(),
		spectral_data::XYZ_CMF_CIE_1931_2_degree_X().data(), 
		numXyzCmfPoints);

	kernel_Y_D65 = SpectralData::calcPiecewiseAveraged(
		spectral_data::XYZ_CMF_CIE_1931_2_degree_wavelengths_nm().data(),
		spectral_data::XYZ_CMF_CIE_1931_2_degree_Y().data(), 
		numXyzCmfPoints);

	kernel_Z_D65 = SpectralData::calcPiecewiseAveraged(
		spectral_data::XYZ_CMF_CIE_1931_2_degree_wavelengths_nm().data(),
		spectral_data::XYZ_CMF_CIE_1931_2_degree_Z().data(), 
		numXyzCmfPoints);

	// normalizing

	// multiplier of Riemann Sum
	//
	kernel_X_D65.mulLocal(SampledSpectralStrength::LAMBDA_INTERVAL_NM);
	kernel_Y_D65.mulLocal(SampledSpectralStrength::LAMBDA_INTERVAL_NM);
	kernel_Z_D65.mulLocal(SampledSpectralStrength::LAMBDA_INTERVAL_NM);
	
	// scaled to match D65
	//
	kernel_X_D65.mulLocal(0.95047_r / kernel_X_D65.dot(SPD_D65));
	kernel_Y_D65.mulLocal(1.00000_r / kernel_Y_D65.dot(SPD_D65));
	kernel_Z_D65.mulLocal(1.08883_r / kernel_Z_D65.dot(SPD_D65));

	// Constructing sampled SPD for Smits' algorithm.
	
	const std::size_t numSmitsPoints = std::tuple_size<spectral_data::ArraySmits>::value;

	SPD_Smits_E_white   = SpectralData::calcPiecewiseAveraged(
		spectral_data::smits_linear_sRGB_to_spectrum_E_wavelengths_nm().data(),
		spectral_data::smits_linear_sRGB_to_spectrum_E_white().data(),
		numSmitsPoints);

	SPD_Smits_E_cyan    = SpectralData::calcPiecewiseAveraged(
		spectral_data::smits_linear_sRGB_to_spectrum_E_wavelengths_nm().data(),
		spectral_data::smits_linear_sRGB_to_spectrum_E_cyan().data(),
		numSmitsPoints);

	SPD_Smits_E_magenta = SpectralData::calcPiecewiseAveraged(
		spectral_data::smits_linear_sRGB_to_spectrum_E_wavelengths_nm().data(),
		spectral_data::smits_linear_sRGB_to_spectrum_E_magenta().data(),
		numSmitsPoints);

	SPD_Smits_E_yellow  = SpectralData::calcPiecewiseAveraged(
		spectral_data::smits_linear_sRGB_to_spectrum_E_wavelengths_nm().data(),
		spectral_data::smits_linear_sRGB_to_spectrum_E_yellow().data(),
		numSmitsPoints);

	SPD_Smits_E_red     = SpectralData::calcPiecewiseAveraged(
		spectral_data::smits_linear_sRGB_to_spectrum_E_wavelengths_nm().data(),
		spectral_data::smits_linear_sRGB_to_spectrum_E_red().data(),
		numSmitsPoints);

	SPD_Smits_E_green   = SpectralData::calcPiecewiseAveraged(
		spectral_data::smits_linear_sRGB_to_spectrum_E_wavelengths_nm().data(),
		spectral_data::smits_linear_sRGB_to_spectrum_E_green().data(),
		numSmitsPoints);

	SPD_Smits_E_blue    = SpectralData::calcPiecewiseAveraged(
		spectral_data::smits_linear_sRGB_to_spectrum_E_wavelengths_nm().data(),
		spectral_data::smits_linear_sRGB_to_spectrum_E_blue().data(),
		numSmitsPoints);

	PH_ASSERT(isInitialized(true));
}

Vector3R ColorSpace::SPD_to_CIE_XYZ_D65(const SampledSpectralStrength& spd)
{
	PH_ASSERT(isInitialized());

	return Vector3R(kernel_X_D65.dot(spd),
	                kernel_Y_D65.dot(spd),
	                kernel_Z_D65.dot(spd));
}

void ColorSpace::linear_sRGB_to_SPD(const Vector3R& color, SampledSpectralStrength* const out_spd)
{
	PH_ASSERT(isInitialized());
	PH_ASSERT(out_spd != nullptr);

	const real r = color.x;
	const real g = color.y;
	const real b = color.z;

	out_spd->set(0);

	// The following steps mix in primary colors only as needed.

	// when R is minimum
	if(r <= g && r <= b)
	{
		out_spd->addLocal(SPD_Smits_E_white * r);
		if(g <= b)
		{
			out_spd->addLocal(SPD_Smits_E_cyan * (g - r));
			out_spd->addLocal(SPD_Smits_E_blue * (b - g));
		}
		else
		{
			out_spd->addLocal(SPD_Smits_E_cyan * (b - r));
			out_spd->addLocal(SPD_Smits_E_green * (g - b));
		}
	}
	// when G is minimum
	else if(g <= r && g <= b)
	{
		out_spd->addLocal(SPD_Smits_E_white * g);
		if(r <= b)
		{
			out_spd->addLocal(SPD_Smits_E_magenta * (r - g));
			out_spd->addLocal(SPD_Smits_E_blue * (b - r));
		}
		else
		{
			out_spd->addLocal(SPD_Smits_E_magenta * (b - g));
			out_spd->addLocal(SPD_Smits_E_red * (r - b));
		}
	}
	// when B is minimum
	else
	{
		out_spd->addLocal(SPD_Smits_E_white * b);
		if(r <= g)
		{
			out_spd->addLocal(SPD_Smits_E_yellow * (r - b));
			out_spd->addLocal(SPD_Smits_E_green * (g - r));
		}
		else
		{
			out_spd->addLocal(SPD_Smits_E_yellow * (g - b));
			out_spd->addLocal(SPD_Smits_E_red * (r - g));
		}
	}

	// TODO: ensure energy conservation?
}

void ColorSpace::sRGB_to_SPD(const Vector3R& color, SampledSpectralStrength* const out_spd)
{
	linear_sRGB_to_SPD(sRGB_to_linear_sRGB(color), out_spd);
}

const SampledSpectralStrength& ColorSpace::get_D65_SPD()
{
	PH_ASSERT(isInitialized());

	return SPD_D65;
}

}// end namespace ph