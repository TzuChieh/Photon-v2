#pragma once

#include "Engine/Math/TVector3.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Actor/SDLExtension/sdl_color_enums.h"
#include "Engine/Math/Color/color_basics.h"

#include <string_view>

namespace ph::sdl
{

math::Spectrum tristimulus_to_spectrum(
	const math::TVector3<math::ColorValue>& tristimulus,
	math::EColorSpace colorSpace,
	math::EColorUsage usage);

/*! @brief Load an SDL spectrum using tag and usage-aware color semantics.
- Raw represents numeric components or wavelength samples, not light or an energy fraction.
- In a tristimulus working color space, Raw triples are stored directly; any tristimulus tag is ignored.
- In a spectral working color space, tagged Raw triples use the tagged tristimulus color space;
  untagged Raw triples are interpreted as linear sRGB.
- An untagged Raw scalar fills every working color space component.
- EMR and ECF triples use the tagged color space, or linear sRGB when untagged.
- SPD input is converted to the working color space for Raw, EMR, and ECF.
*/
///@{
math::Spectrum load_spectrum(
	std::string_view sdlSpectrumStr,
	std::string_view tag,
	math::EColorUsage usage);

math::Spectrum load_spectrum(
	std::string_view sdlSpectrumStr,
	math::EColorSpace colorSpace,
	math::EColorUsage usage);
///@}

/*! @brief Save a spectrum in the working color space with usage-aware tagging.
- A tristimulus working color space saves components unchanged and tags the working color space.
- A spectral working color space saves Raw, EMR, and ECF values as SPD samples.
@note Reloading with the same working color space preserves components. If the working color space
changes, load rules determine conversion; component round trips are not guaranteed.

@param out_sdlSpectrumStr Stores the saved spectrum. Appends to existing data.
@param out_tag Stores the saved tag (if any). Appends to existing data.
*/
void save_spectrum(
	const math::Spectrum& spectrum,
	math::EColorUsage usage,
	std::string& out_sdlSpectrumStr,
	std::string& out_tag);

}// end namespace ph::sdl
