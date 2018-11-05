#include "Core/Engine.h"
#include "Frame/TFrame.h"
#include "Frame/FrameProcessor.h"
#include "Frame/Operator/JRToneMapping.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Common/Logger.h"

namespace ph
{

namespace
{
	const Logger logger(LogSender("Engine"));
}

Engine::Engine() : 
	m_renderer(nullptr)
{
	setNumRenderThreads(1);
}

void Engine::enterCommand(const std::string& commandFragment)
{
	m_parser.enter(commandFragment, m_data);
}

void Engine::update()
{
	// HACK
	m_data.update(0.0_r);

	// HACK
	std::shared_ptr<FrameProcessor> processor = std::make_shared<FrameProcessor>();
	processor->appendOperator(std::make_shared<JRToneMapping>());
	m_filmSet.setProcessor(EAttribute::LIGHT_ENERGY, processor);
	m_filmSet.setProcessor(EAttribute::NORMAL, processor);

	m_renderer = m_data.getRenderer();
	m_renderer->setNumWorkers(m_numRenderThreads);
	m_renderer->update(m_data);
}

void Engine::render()
{
	// HACK
	m_renderer->render();
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

	logger.log("number of render threads set to " + std::to_string(numThreads));
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
	// HACK
	RenderProgress progress = m_renderer->asyncQueryRenderProgress();
	RenderState state = m_renderer->asyncQueryRenderState();
	*out_percentageProgress = progress.getPercentageProgress();
	float32 samplesPerMs = state.getRealState(0);
	*out_samplesPerSecond = samplesPerMs * 1000;
}

void Engine::setWorkingDirectory(const Path& path)
{
	m_parser.setWorkingDirectory(path);
}

}// end namespace ph