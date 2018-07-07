#include "Core/Renderer/Sampling/SamplingFilmSet.h"

namespace ph
{

SamplingFilmSet::SamplingFilmSet() : 
	m_tagToFilm(),
	m_films{}
{}

SamplingFilmSet::SamplingFilmSet(SamplingFilmSet&& other) : 
	m_tagToFilm(std::move(other.m_tagToFilm)),
	m_films(std::move(other.m_films))
{}

SamplingFilmSet& SamplingFilmSet::operator = (SamplingFilmSet&& rhs)
{
	m_tagToFilm = std::move(rhs.m_tagToFilm);
	m_films     = std::move(rhs.m_films);

	return *this;
}

}// end namespace ph