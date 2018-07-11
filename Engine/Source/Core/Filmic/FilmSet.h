#pragma once

#include "Core/Filmic/filmic_fwd.h"
#include "Frame/FrameProcessor.h"
#include "Core/Renderer/EAttribute.h"

#include <memory>
#include <array>
#include <cstddef>

namespace ph
{

// TODO: change name to FrameProcessorSet
class FilmSet final
{
public:
	const FrameProcessor* getProcessor(EAttribute tag) const;
	void setProcessor(EAttribute tag, const std::shared_ptr<FrameProcessor>& processor);

private:
	std::array<
		std::shared_ptr<SpectralSamplingFilm>,
		static_cast<std::size_t>(EAttribute::SIZE)
	> m_films;

	std::array<
		std::shared_ptr<FrameProcessor>, 
		static_cast<std::size_t>(EAttribute::SIZE)
	> m_processors;

	static std::size_t tagToIndex(EAttribute tag);
};

}// end namespace ph