#pragma once

#include "Math/Color/TSpectrumBase.h"

namespace ph::math
{

template<EColorSpace COLOR_SPACE, typename T>
class TTristimulusSpectrum final :
	public TSpectrumBase<TTristimulusSpectrum<COLOR_SPACE, T>, COLOR_SPACE, T, 3>
{
private:
	using Base = TSpectrumBase<TTristimulusSpectrum<COLOR_SPACE, T>, COLOR_SPACE, T, 3>;

protected:
	using Base::m;

public:
	inline TTristimulusSpectrum() = default;
	inline TTristimulusSpectrum(const TTristimulusSpectrum& other) = default;
	inline TTristimulusSpectrum(TTristimulusSpectrum&& other) = default;
	inline TTristimulusSpectrum& operator = (const TTristimulusSpectrum& rhs) = default;
	inline TTristimulusSpectrum& operator = (TTristimulusSpectrum&& rhs) = default;
	inline ~TTristimulusSpectrum() = default;

	using Base::Base;

	template<typename U>
	TTristimulusSpectrum(const TTristimulusSpectrum<COLOR_SPACE, U>& other);
};

}// end namespace ph::math

#include "Math/Color/TTristimulusSpectrum.ipp"
