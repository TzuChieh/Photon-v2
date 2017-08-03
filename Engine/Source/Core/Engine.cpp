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
	// HACK
	m_renderer->setNumRenderThreads(numThreads);
}

float32 Engine::queryPercentageProgress() const
{
	// HACK
	return m_renderer->asyncQueryPercentageProgress();
}

float32 Engine::querySampleFrequency() const
{
	// HACK
	return m_renderer->asyncQuerySampleFrequency();
}

}// end namespace ph