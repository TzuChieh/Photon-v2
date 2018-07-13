#include "Core/Engine.h"
#include "Frame/TFrame.h"
#include "Core/Renderer/SamplingRenderer.h"
#include "Frame/FrameProcessor.h"
#include "Frame/Operator/JRToneMapping.h"
#include "Core/Filmic/TSamplingFilm.h"

namespace ph
{

Engine::Engine() : 
	m_renderer(nullptr), m_numRenderThreads(1)
{}

void Engine::enterCommand(const std::string& commandFragment)
{
	m_parser.enter(commandFragment, m_description);
}

void Engine::update()
{
	// HACK
	m_description.update(0.0_r);

	// HACK
	std::shared_ptr<FrameProcessor> processor = std::make_shared<FrameProcessor>();
	processor->appendOperator(std::make_shared<JRToneMapping>());
	m_filmSet.setProcessor(EAttribute::LIGHT_ENERGY, processor);
	m_filmSet.setProcessor(EAttribute::NORMAL, processor);

	m_renderer = m_description.getRenderer();
	m_renderer->setNumRenderThreads(m_numRenderThreads);
}

void Engine::render()
{
	// HACK
	m_renderer->render(m_description);
}

void Engine::developFilm(
	HdrRgbFrame&     out_frame, 
	const EAttribute attribute,
	const bool       applyPostProcessing)
{
	m_renderer->develop(out_frame, attribute);

	if(applyPostProcessing)
	{
		const FrameProcessor* processor = m_filmSet.getProcessor(attribute);
		processor->process(out_frame);
	}
}

TVector2<int64> Engine::getFilmDimensionPx() const
{
	return {m_renderer->getRenderWidthPx(), m_renderer->getRenderHeightPx()};
}

void Engine::setNumRenderThreads(const uint32 numThreads)
{
	m_numRenderThreads = numThreads;
}

ERegionStatus Engine::asyncPollUpdatedRegion(Region* const out_region) const
{
	return m_renderer->asyncPollUpdatedRegion(out_region);
}

void Engine::asyncDevelopFilmRegion(
	HdrRgbFrame&            out_frame,
	const Region& region,
	const EAttribute        attribute,
	const bool              applyPostProcessing) const
{
	m_renderer->asyncDevelopFilmRegion(out_frame, region, attribute);

	if(applyPostProcessing)
	{
		const FrameProcessor* processor = m_filmSet.getProcessor(attribute);
		PH_ASSERT(processor);
		processor->process(out_frame);
	}
}

void Engine::asyncQueryStatistics(
	float32* const out_percentageProgress,
	float32* const out_samplesPerSecond) const
{
	m_renderer->asyncQueryStatistics(out_percentageProgress, out_samplesPerSecond);
}

void Engine::setWorkingDirectory(const Path& path)
{
	m_parser.setWorkingDirectory(path);
}

}// end namespace ph