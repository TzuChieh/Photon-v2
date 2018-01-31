#include "Core/Engine.h"
#include "Frame/TFrame.h"
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

void Engine::update()
{
	// HACK
	m_description.update(0.0_r);
}

void Engine::render()
{
	// HACK
	m_renderer->render(m_description);
}

void Engine::developFilm(HdrRgbFrame& out_frame)
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

ERegionStatus Engine::asyncPollUpdatedRegion(Renderer::Region* const out_region) const
{
	return m_renderer->asyncPollUpdatedRegion(out_region);
}

void Engine::asyncDevelopFilmRegion(HdrRgbFrame& out_frame,
                                    const Renderer::Region& region) const
{
	m_renderer->asyncDevelopFilmRegion(out_frame, region);
}

void Engine::asyncQueryStatistics(float32* const out_percentageProgress,
                                  float32* const out_samplesPerSecond) const
{
	m_renderer->asyncQueryStatistics(out_percentageProgress, out_samplesPerSecond);
}

void Engine::setWorkingDirectory(const Path& path)
{
	m_parser.setWorkingDirectory(path);
}

}// end namespace ph