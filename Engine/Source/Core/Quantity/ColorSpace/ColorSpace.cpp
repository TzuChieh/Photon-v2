#include "Core/Quantity/ColorSpace.h"
#include "Common/assertion.h"
#include "Core/Quantity/ColorSpace/spectral_data.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/Quantity/SpectralData.h"

#include <array>
#include <iostream>

namespace ph
{

SampledSpectralStrength ColorSpace::SPD_E;
SampledSpectralStrength ColorSpace::SPD_D65;

SampledSpectralStrength ColorSpace::kernel_X_E;
SampledSpectralStrength ColorSpace::kernel_Y_E;
SampledSpectralStrength ColorSpace::kernel_Z_E;
SampledSpectralStrength ColorSpace::kernel_X_D65;
SampledSpectralStrength ColorSpace::kernel_Y_D65;
SampledSpectralStrength ColorSpace::kernel_Z_D65;

SampledSpectralStrength ColorSpace::SPD_Smits_E_white;
SampledSpectralStrength ColorSpace::SPD_Smits_E_cyan;
SampledSpectralStrength ColorSpace::SPD_Smits_E_magenta;
SampledSpectralStrength ColorSpace::SPD_Smits_E_yellow;
SampledSpectralStrength ColorSpace::SPD_Smits_E_red;
SampledSpectralStrength ColorSpace::SPD_Smits_E_green;
SampledSpectralStrength ColorSpace::SPD_Smits_E_blue;

void ColorSpace::init()
{
	PH_ASSERT_MSG(!isInitialized(), "ColorSpace is already initialized");

	SPD_E = SampledSpectralStrength(1.0_r);

	// Construct sampled D65 spectrum and normalize it by making its largest
	// value equals to 1.

	SPD_D65 = SpectralData::calcPiecewiseAveraged(
		spectral_data::CIE_D65_wavelengths_nm().data(),
		spectral_data::CIE_D65_values().data(),
		std::tuple_size<spectral_data::ArrayD65>::value);

	// Normalizing SPDs of standard illuminants. Each normalized SPD represents
	// the expected amount of energy that 1 watt of total energy would distribute
	// on each wavelength interval (implying that each SPD should sum to 1).
	// 
	SPD_E.divLocal(SPD_E.sum());
	SPD_D65.divLocal(SPD_D65.sum());

	// Sample XYZ color matching functions first, then later normalize it so 
	// that dotting them with sampled E/D65 spectrum is equivalent to computing
	// (X, Y, Z) tristimulus values and will yield (1, 1, 1)/(0.95047, 1, 1.08883).

	// sampling XYZ CMF

	const std::size_t numXyzCmfPoints = std::tuple_size<spectral_data::ArrayXYZCMF>::value;

	const SampledSpectralStrength& sampledCmfX = SpectralData::calcPiecewiseAveraged(
		spectral_data::XYZ_CMF_CIE_1931_2_degree_wavelengths_nm().data(),
		spectral_data::XYZ_CMF_CIE_1931_2_degree_X().data(), 
		numXyzCmfPoints);

	const SampledSpectralStrength& sampledCmfY = SpectralData::calcPiecewiseAveraged(
		spectral_data::XYZ_CMF_CIE_1931_2_degree_wavelengths_nm().data(),
		spectral_data::XYZ_CMF_CIE_1931_2_degree_Y().data(), 
		numXyzCmfPoints);

	const SampledSpectralStrength& sampledCmfZ = SpectralData::calcPiecewiseAveraged(
		spectral_data::XYZ_CMF_CIE_1931_2_degree_wavelengths_nm().data(),
		spectral_data::XYZ_CMF_CIE_1931_2_degree_Z().data(), 
		numXyzCmfPoints);

	kernel_X_E   = sampledCmfX;
	kernel_Y_E   = sampledCmfY;
	kernel_Z_E   = sampledCmfZ;
	kernel_X_D65 = sampledCmfX;
	kernel_Y_D65 = sampledCmfY;
	kernel_Z_D65 = sampledCmfZ;

	// normalizing

	// multiplier of Riemann Sum
	//
	kernel_X_E.mulLocal(SampledSpectralStrength::LAMBDA_INTERVAL_NM);
	kernel_Y_E.mulLocal(SampledSpectralStrength::LAMBDA_INTERVAL_NM);
	kernel_Z_E.mulLocal(SampledSpectralStrength::LAMBDA_INTERVAL_NM);
	kernel_X_D65.mulLocal(SampledSpectralStrength::LAMBDA_INTERVAL_NM);
	kernel_Y_D65.mulLocal(SampledSpectralStrength::LAMBDA_INTERVAL_NM);
	kernel_Z_D65.mulLocal(SampledSpectralStrength::LAMBDA_INTERVAL_NM);
	
	// scaled to match E/D65
	//
	kernel_X_E.mulLocal(1.0_r / kernel_X_E.dot(SPD_E));
	kernel_Y_E.mulLocal(1.0_r / kernel_Y_E.dot(SPD_E));
	kernel_Z_E.mulLocal(1.0_r / kernel_Z_E.dot(SPD_E));
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

Vector3R ColorSpace::SPD_to_CIE_XYZ_E(const SampledSpectralStrength& spd)
{
	PH_ASSERT(isInitialized());

	return Vector3R(kernel_X_E.dot(spd),
	                kernel_Y_E.dot(spd),
	                kernel_Z_E.dot(spd));
}

}// end namespace ph