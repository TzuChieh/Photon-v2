#pragma once

/*! @file
Various spectral dataset are available here. Note that many dataset are provided as functions rather than
directly in the form of arrays. It is done in this way so that how the data is stored is opaque to the 
users. The data, under the hood, may be hard-coded directly in the source or lazily loaded from disk.
*/

#include <array>
#include <cstddef>

namespace ph::math::spectral_data
{

using ArraySmits  = std::array<double, 40>;
using ArrayXYZCMF = std::array<double, 471>;
using ArrayD65    = std::array<double, 531>;

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

}// end namespace ph::math::spectral_data
