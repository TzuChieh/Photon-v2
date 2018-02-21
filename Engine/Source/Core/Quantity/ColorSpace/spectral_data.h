#pragma once

#include "Common/primitive_type.h"

#include <array>
#include <cstddef>

namespace ph
{

namespace spectral_data
{

typedef std::array<real, 64> Array64R;

const Array64R& smits_linear_sRGB_to_spectrum_wavelengths_nm();
const Array64R& smits_linear_sRGB_to_spectrum_white();
const Array64R& smits_linear_sRGB_to_spectrum_cyan();
const Array64R& smits_linear_sRGB_to_spectrum_magenta();
const Array64R& smits_linear_sRGB_to_spectrum_yellow();
const Array64R& smits_linear_sRGB_to_spectrum_red();
const Array64R& smits_linear_sRGB_to_spectrum_green();
const Array64R& smits_linear_sRGB_to_spectrum_blue();

}// end namespace spectral_data

}// end namespace ph