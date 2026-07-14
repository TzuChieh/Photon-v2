#pragma once

#include "Engine/Core/Filmic/TSamplingFilm.h"

#include <memory>

namespace ph
{

template<typename Sample>
struct SamplingFilmLayer final
{
	std::unique_ptr<TSamplingFilm<Sample>> film;
};

}// end namespace ph
