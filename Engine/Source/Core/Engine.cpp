#include "Core/Engine.h"
#include "PostProcess/Frame.h"

namespace ph
{

Engine::Engine() : 
	//visualWorld(), 
	m_renderer()
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

	m_renderer.render(m_description);
}

void Engine::developFilm(Frame* const out_frame)
{
	m_description.getFilm()->develop(out_frame);
}

void Engine::setNumRenderThreads(const std::size_t numThreads)
{
	m_renderer.setNumRenderThreads(static_cast<uint32>(numThreads));
}

float32 Engine::queryPercentageProgress() const
{
	return m_renderer.queryPercentageProgress();
}

float32 Engine::querySampleFrequency() const
{
	return m_renderer.querySampleFrequency();
}

}// end namespace ph