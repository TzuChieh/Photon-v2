#pragma once

#include "Math/TVector3.h"
#include "Math/Color/Spectrum.h"
#include "Actor/SDLExtension/sdl_color_enums.h"
#include "Math/Color/color_basics.h"

#include <string>

namespace ph::sdl
{

math::Spectrum tristimulus_to_spectrum(
	const math::TVector3<math::ColorValue>& tristimulus, 
	math::EColorSpace colorSpace, 
	math::EColorUsage usage);

math::Spectrum load_spectrum(
	const std::string& sdlSpectrumStr, 
	const std::string& tag,
	math::EColorUsage usage);

void save_spectrum(
	const math::Spectrum& spectrum,
	std::string* out_sdlSpectrumStr,
	std::string* out_tag);

}// end namespace ph::sdl
