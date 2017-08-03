#include "Core/Engine.h"
#include "PostProcess/Frame.h"
#include "Core/Renderer/BulkRenderer.h"

namespace ph
{

Engine::Engine() : 
	m_renderer(std::make_unique<BulkRenderer>())
{

}

void Engine::enterCommand(const std::string& commandFragment)
{
	m_parser.enter(commandFragment, m_description);
}

void Engine::render()
{
	// HACK
	m_description.update(0.0_r);

	// HACK
	m_renderer->render(m_description);
}

void Engine::developFilm(Frame& out_frame)
{
	m_description.getFilm()->develop(out_frame);
}

TVector2<int64> Engine::getFilmDimensionPx() const
{
	return m_description.getFilm()->getActualResPx();
}

void Engine::setNumRenderThreads(const uint32 numThreads)
{
	m_renderer->setNumRenderThreads(numThreads);
}

float32 Engine::asyncQueryPercentageProgress() const
{
	return m_renderer->asyncQueryPercentageProgress();
}

float32 Engine::asyncQuerySampleFrequency() const
{
	return m_renderer->asyncQuerySampleFrequency();
}

ERegionStatus Engine::asyncPollUpdatedRegion(Renderer::Region* const out_region) const
{
	return m_renderer->asyncPollUpdatedRegion(out_region);
}

void Engine::asyncDevelopFilmRegion(Frame& out_frame, 
                                    const Renderer::Region& region) const
{
	m_renderer->asyncDevelopFilmRegion(out_frame, region);
}

}// end namespace ph