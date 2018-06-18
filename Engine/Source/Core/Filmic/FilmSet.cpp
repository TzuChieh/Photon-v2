#include "Core/Filmic/FilmSet.h"
#include "Common/assertion.h"

namespace ph
{

const FrameProcessor* FilmSet::getProcessor(const EAttribute tag) const
{
	PH_ASSERT(tagToIndex(tag) < m_processors.size());

	return m_processors[tagToIndex(tag)].get();
}

void FilmSet::setProcessor(const EAttribute tag, const std::shared_ptr<FrameProcessor>& processor)
{
	PH_ASSERT(tagToIndex(tag) < m_processors.size());

	m_processors[tagToIndex(tag)] = processor;
}

std::size_t FilmSet::tagToIndex(const EAttribute tag)
{
	return static_cast<std::size_t>(tag);
}

}// end namespace ph