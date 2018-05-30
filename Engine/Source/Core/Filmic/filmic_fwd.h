#pragma once

#include "Core/Quantity/SpectralStrength.h"

namespace ph
{

class Film;

template<typename Sample>
class TSamplingFilm;

using SpectralSamplingFilm = TSamplingFilm<SpectralStrength>;

class HdrRgbFilm;

}// end namespace ph