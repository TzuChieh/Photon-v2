#pragma once

#include "Math/Color/TTristimulusSpectrum.h"

namespace ph::math
{

template<EColorSpace COLOR_SPACE, typename T>
template<typename U>
inline TTristimulusSpectrum<COLOR_SPACE, T>::TTristimulusSpectrum(const TTristimulusSpectrum<COLOR_SPACE, U>& other) :
	TTristimulusSpectrum(other.getColorValues())
{}

}// end namespace ph::math
