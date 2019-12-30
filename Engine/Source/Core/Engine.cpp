#include "Core/Engine.h"
#include "Frame/TFrame.h"
#include "Frame/FrameProcessor.h"
#include "Frame/Operator/JRToneMapping.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Common/Logger.h"
#include "Math/Geometry/TAABB2D.h"
#include "Utility/Timer.h"

#include <fstream>
#include <string>

namespace ph
{

namespace
{
	Logger logger(LogSender("Engine"));
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

bool Engine::loadCommands(const Path& filePath)
{
	Timer timer;
	timer.start();

	std::ifstream commandFile;
	commandFile.open(filePath.toAbsoluteString(), std::ios::in);
	if(!commandFile.is_open())
	{
		logger.log(ELogLevel::WARNING_MAX,
			"command file <" + filePath.toAbsoluteString() + "> opening failed");
		return false;
	}
	else
	{
		logger.log(ELogLevel::NOTE_MAX,
			"loading command file <" + filePath.toAbsoluteString() + ">");

		std::string lineCommand;
		while(commandFile.good())
		{
			std::getline(commandFile, lineCommand);
			lineCommand += '\n';

			enterCommand(lineCommand);
		}
		enterCommand("->");

		timer.finish();
		logger.log(ELogLevel::NOTE_MAX,
			"command file loaded, time elapsed = " + std::to_string(timer.getDeltaMs()) + " ms");

		return true;
	}
}

void Engine::update()
{
	// HACK
	m_data.update(0.0_r);

	// HACK
	//m_id = m_frameProcessor.addPipeline();
	//m_frameProcessor.getPipeline(m_id)->appendOperator(std::make_unique<JRToneMapping>());
	/*m_filmSet.setProcessor(EAttribute::LIGHT_ENERGY, processor);
	m_filmSet.setProcessor(EAttribute::NORMAL, processor);*/

	m_renderer = m_data.getRenderer();
	m_renderer->setNumWorkers(m_numRenderThreads);
	m_renderer->update(m_data);
}

void Engine::render()
{
	// HACK
	m_renderer->render();
}

void Engine::retrieveFrame(
	const std::size_t layerIndex,
	HdrRgbFrame&      out_frame,
	const bool        applyPostProcessing)
{
	m_renderer->retrieveFrame(layerIndex, out_frame);

	if(applyPostProcessing)
	{
		//m_frameProcessor.process(out_frame, m_id);
		// HACK
		JRToneMapping().operate(out_frame);
	}
}

math::TVector2<int64> Engine::getFilmDimensionPx() const
{
	return {m_renderer->getRenderWidthPx(), m_renderer->getRenderHeightPx()};
}

void Engine::setNumRenderThreads(const uint32 numThreads)
{
	if(m_numRenderThreads == numThreads)
	{
		return;
	}

	m_numRenderThreads = numThreads;

	logger.log("number of render threads set to " + std::to_string(numThreads));
}

ERegionStatus Engine::asyncPollUpdatedRegion(Region* const out_region) const
{
	return m_renderer->asyncPollUpdatedRegion(out_region);
}

void Engine::asyncPeekFrame(
	const std::size_t layerIndex,
	const Region&     region,
	HdrRgbFrame&      out_frame,
	const bool        applyPostProcessing) const
{
	m_renderer->asyncPeekFrame(layerIndex, region, out_frame);

	if(applyPostProcessing)
	{
		//m_frameProcessor.process(out_frame, m_id);
		// HACK
		JRToneMapping().operateLocal(out_frame, math::TAABB2D<uint32>(region));
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
	*out_samplesPerSecond = state.getRealState(0);
}

void Engine::setWorkingDirectory(const Path& path)
{
	m_parser.setWorkingDirectory(path);
}

}// end namespace ph
