#include "Core/Engine.h"
#include "Frame/TFrame.h"
#include "Frame/FrameProcessor.h"
#include "Frame/Operator/JRToneMapping.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Common/Logger.h"
#include "Math/Geometry/TAABB2D.h"
#include "Utility/Timer.h"
#include "EngineEnv/Session/RenderSession.h"
#include "EngineEnv/CoreCookingContext.h"
#include "Core/Receiver/Receiver.h"
#include "Core/Renderer/Renderer.h"
#include "Math/Transform/RigidTransform.h"

#include <fstream>
#include <string>

namespace ph
{

namespace
{
	Logger logger(LogSender("Engine"));
}

Engine::Engine() : 
	m_cooked(),
	m_visualWorld()
{
	setNumRenderThreads(1);
}

void Engine::enterCommand(const std::string& commandFragment)
{
	m_parser.enter(commandFragment, m_rawScene);
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

		timer.finish();
		logger.log(ELogLevel::NOTE_MAX,
			"command file loaded, time elapsed = " + std::to_string(timer.getDeltaMs()) + " ms");

		return true;
	}
}

void Engine::update()
{
	// Process and wait all unfinished commands
	m_parser.flush(m_rawScene);

	std::shared_ptr<RenderSession> renderSession;
	{
		// TODO: means to specify the requested session

		std::vector<std::shared_ptr<RenderSession>> renderSessions = m_rawScene.getResources<RenderSession>();
		if(renderSessions.empty())
		{
			logger.log(ELogLevel::FATAL_ERROR,
				"require at least a render session; engine failed to update");
			return;
		}

		if(renderSessions.size() > 1)
		{
			logger.log(ELogLevel::FATAL_ERROR,
				"more than 1 render session specified, taking the last one");
		}

		renderSession = renderSessions.back();
	}

	PH_ASSERT(renderSession);

	CoreCookingContext coreCtx;
	renderSession->applyToContext(coreCtx);

	std::vector<std::shared_ptr<CoreSdlResource>> coreResources = renderSession->gatherResources(m_rawScene);
	for(auto& coreResource : coreResources)
	{
		coreResource->cook(coreCtx, m_cooked);
	}

	// TODO: better way to check/assign cooked renderer
	if(!m_cooked.getRenderer())
	{
		logger.log(ELogLevel::FATAL_ERROR,
			"no renderer present");
		return;
	}

	// TODO: not using hacky way to get receiver position
	PH_ASSERT(m_cooked.getReceiver());
	math::Vector3R receiverPos;
	m_cooked.getReceiver()->getReceiverToWorld().transformP({0, 0, 0}, &receiverPos);
	m_visualWorld.setReceiverPosition(receiverPos);

	m_visualWorld.cook(m_rawScene, coreCtx);


	m_coreData.getRenderer()->setNumWorkers(m_numRenderThreads);
	m_coreData.getRenderer()->update(m_coreData);
}

void Engine::render()
{
	getRenderer()->render();
}

void Engine::retrieveFrame(
	const std::size_t layerIndex,
	HdrRgbFrame&      out_frame,
	const bool        applyPostProcessing)
{
	Renderer* const renderer = getRenderer();
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
	Renderer* const renderer = getRenderer();
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
	return getRenderer()->asyncPollUpdatedRegion(out_region);
}

void Engine::asyncPeekFrame(
	const std::size_t layerIndex,
	const Region&     region,
	HdrRgbFrame&      out_frame,
	const bool        applyPostProcessing) const
{
	getRenderer()->asyncPeekFrame(layerIndex, region, out_frame);

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
	Renderer* const renderer = getRenderer();
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
