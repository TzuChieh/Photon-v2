#pragma once

#include "Common/assertion.h"
#include "Core/Filmic/filmic_fwd.h"

#include <functional>

namespace ph
{

template<typename Sample>
class TMergeableFilm
{
public:
	TMergeableFilm(TSamplingFilm<Sample>* film, std::function<void()> merger);

	void merge();
	TSamplingFilm<Sample>* get() const;

private:
	TSamplingFilm<Sample>* m_film;
	std::function<void()>  m_merger;
};

// In-header Implementation:

template<typename Sample>
inline TMergeableFilm<Sample>::TMergeableFilm(
	TSamplingFilm<Sample>* const film, 
	std::function<void()>        merger) : 

	m_film  (film),
	m_merger(std::move(merger))
{
	PH_ASSERT(film);
	PH_ASSERT(merger);
}

template<typename Sample>
inline void TMergeableFilm<Sample>::merge()
{
	m_merger();
}

template<typename Sample>
inline TSamplingFilm<Sample>* TMergeableFilm<Sample>::get() const
{
	return m_film;
}

}// end namespace ph