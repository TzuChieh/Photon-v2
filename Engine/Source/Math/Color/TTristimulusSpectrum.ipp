#pragma once

#include "Math/Color/TTristimulusSpectrum.h"

namespace ph::math
{

template<EColorSpace COLOR_SPACE, typename T>
inline TTristimulusSpectrum<COLOR_SPACE, T>::TTristimulusSpectrum(const T c0, const T c1, const T c2) :
	Base(std::array<T, 3>{c0, c1, c2})
{}

template<EColorSpace COLOR_SPACE, typename T>
template<typename U>
inline TTristimulusSpectrum<COLOR_SPACE, T>::TTristimulusSpectrum(const TTristimulusSpectrum<COLOR_SPACE, U>& other) :
	TTristimulusSpectrum(
		static_cast<T>(other[0]),
		static_cast<T>(other[1]),
		static_cast<T>(other[2]))
{}

}// end namespace ph::math
