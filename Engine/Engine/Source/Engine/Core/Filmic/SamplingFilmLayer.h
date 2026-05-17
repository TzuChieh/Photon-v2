#pragma once

#include "Engine/Core/Filmic/TSamplingFilm.h"

#include <memory>
#include <string>

namespace ph
{

template<typename Sample>
struct SamplingFilmLayer final
{
	std::unique_ptr<TSamplingFilm<Sample>> film;
	std::string name;
};

}// end namespace ph
