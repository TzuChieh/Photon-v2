#include "Core/Engine.h"
#include "Frame/TFrame.h"
#include "Frame/FrameProcessor.h"
#include "Frame/Operator/JRToneMapping.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Common/Logger.h"
#include "Math/Geometry/TAABB2D.h"
#include "Utility/Timer.h"
#include "Core/EngineOption.h"
#include "World/CookSettings.h"
#include "Core/Receiver/Receiver.h"
#include "Core/Renderer/Renderer.h"

#include <fstream>
#include <string>

namespace ph
{

namespace
{
	Logger logger(LogSender("Engine"));
}

Engine::Engine() : 
	m_coreData(),
	m_visualWorld()
{
	setNumRenderThreads(1);
}

void Engine::enterCommand(const std::string& commandFragment)
{
	m_parser.enter(commandFragment, m_scene);
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
	if(!m_coreData.gatherFromRaw(m_scene))
	{
		logger.log(ELogLevel::FATAL_ERROR,
			"core raw data may be missing; engine failed to update");
		return;
	}

	m_visualWorld.setReceiverPosition(m_coreData.getReceiver()->getPosition());
	m_visualWorld.setCookSettings(*(m_coreData.getCookSettings()));

	const auto& actors = m_scene.getResources<Actor>();
	for(const auto& actor : actors)
	{
		m_visualWorld.addActor(actor);
	}

	m_visualWorld.cook();

	if(!m_coreData.gatherFromCooked(m_visualWorld))
	{
		logger.log(ELogLevel::FATAL_ERROR,
			"core cooked data may be missing; engine failed to update");
		return;
	}

	m_coreData.getRenderer()->setNumWorkers(m_numRenderThreads);
	m_coreData.getRenderer()->update(m_coreData);
}

void Engine::render()
{
	m_coreData.getRenderer()->render();
}

void Engine::retrieveFrame(
	const std::size_t layerIndex,
	HdrRgbFrame&      out_frame,
	const bool        applyPostProcessing)
{
	Renderer* const renderer = m_coreData.getRenderer();
	PH_ASSERT(renderer);

	renderer->retrieveFrame(layerIndex, out_frame);

	if(applyPostProcessing)
	{
		//m_frameProcessor.process(out_frame, m_id);
		// HACK
		JRToneMapping().operateLocal(out_frame, {{0, 0}, {renderer->getRenderWidthPx(), renderer->getRenderHeightPx()}});
	}
}

math::TVector2<int64> Engine::getFilmDimensionPx() const
{
	Renderer* const renderer = m_coreData.getRenderer();
	PH_ASSERT(renderer);

	return {renderer->getRenderWidthPx(), renderer->getRenderHeightPx()};
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
	return m_coreData.getRenderer()->asyncPollUpdatedRegion(out_region);
}

void Engine::asyncPeekFrame(
	const std::size_t layerIndex,
	const Region&     region,
	HdrRgbFrame&      out_frame,
	const bool        applyPostProcessing) const
{
	m_coreData.getRenderer()->asyncPeekFrame(layerIndex, region, out_frame);

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
	Renderer* const renderer = m_coreData.getRenderer();
	PH_ASSERT(renderer);

	RenderProgress progress = renderer->asyncQueryRenderProgress();
	RenderState state = renderer->asyncQueryRenderState();
	*out_percentageProgress = progress.getPercentageProgress();
	*out_samplesPerSecond = state.getRealState(0);
}

void Engine::setWorkingDirectory(const Path& path)
{
	m_parser.setWorkingDirectory(path);
}

}// end namespace ph
