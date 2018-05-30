#include "Core/Filmic/FilmSet.h"
#include "Common/assertion.h"

namespace ph
{

const SpectralSamplingFilm* FilmSet::getFilm(const EFrameTag tag) const
{
	PH_ASSERT(tagToIndex(tag) < m_films.size());

	return m_films[tagToIndex(tag)].get();
}

const FrameProcessor* FilmSet::getProcessor(const EFrameTag tag) const
{
	PH_ASSERT(tagToIndex(tag) < m_processors.size());

	return m_processors[tagToIndex(tag)].get();
}

void FilmSet::setFilm(const EFrameTag tag, const std::shared_ptr<SpectralSamplingFilm>& film)
{
	PH_ASSERT(tagToIndex(tag) < m_films.size());

	m_films[tagToIndex(tag)] = film;
}

void FilmSet::setProcessor(const EFrameTag tag, const std::shared_ptr<FrameProcessor>& processor)
{
	PH_ASSERT(tagToIndex(tag) < m_processors.size());

	m_processors[tagToIndex(tag)] = processor;
}

std::size_t FilmSet::tagToIndex(const EFrameTag tag)
{
	return static_cast<std::size_t>(tag);
}

}// end namespace ph