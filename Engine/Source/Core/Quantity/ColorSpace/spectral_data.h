#pragma once

#include "Common/primitive_type.h"

#include <array>
#include <cstddef>

namespace ph
{

namespace spectral_data
{

typedef std::array<real, 40>  ArraySmits;
typedef std::array<real, 471> ArrayXYZCMF;
typedef std::array<real, 531> ArrayD65;

// XYZ color matching functions for CIE 1931 2-degree standard.
const ArrayXYZCMF& XYZ_CMF_CIE_1931_2_degree_wavelengths_nm();
const ArrayXYZCMF& XYZ_CMF_CIE_1931_2_degree_X();
const ArrayXYZCMF& XYZ_CMF_CIE_1931_2_degree_Y();
const ArrayXYZCMF& XYZ_CMF_CIE_1931_2_degree_Z();

// CIE standard D65 illuminant spectrum.
const ArrayD65& CIE_D65_wavelengths_nm();
const ArrayD65& CIE_D65_values();

// Data for Smits' RGB to spectrum conversion algorithm.
// The conversion result will resembles a standard illuminant E's spectrum 
// when linear sRGB value reaches its white point (1, 1, 1).
const ArraySmits& smits_linear_sRGB_to_spectrum_E_wavelengths_nm();
const ArraySmits& smits_linear_sRGB_to_spectrum_E_white();
const ArraySmits& smits_linear_sRGB_to_spectrum_E_cyan();
const ArraySmits& smits_linear_sRGB_to_spectrum_E_magenta();
const ArraySmits& smits_linear_sRGB_to_spectrum_E_yellow();
const ArraySmits& smits_linear_sRGB_to_spectrum_E_red();
const ArraySmits& smits_linear_sRGB_to_spectrum_E_green();
const ArraySmits& smits_linear_sRGB_to_spectrum_E_blue();

}// end namespace spectral_data

}// end namespace ph
