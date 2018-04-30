#pragma once

#include "Core/Filmic/Film.h"
#include "Frame/FrameProcessor.h"
#include "Frame/EFrameTag.h"

#include <memory>
#include <array>
#include <cstddef>

namespace ph
{

class FilmSet final
{
public:
	const Film*           getFilm(EFrameTag tag)      const;
	const FrameProcessor* getProcessor(EFrameTag tag) const;

	void setFilm(EFrameTag tag, const std::shared_ptr<Film>& film);
	void setProcessor(EFrameTag tag, const std::shared_ptr<FrameProcessor>& processor);

private:
	std::array<
		std::shared_ptr<Film>, 
		static_cast<std::size_t>(EFrameTag::NUM_ELEMENTS)
	> m_films;

	std::array<
		std::shared_ptr<FrameProcessor>, 
		static_cast<std::size_t>(EFrameTag::NUM_ELEMENTS)
	> m_processors;

	static std::size_t tagToIndex(EFrameTag tag);
};

}// end namespace ph