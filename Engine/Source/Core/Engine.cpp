#include "Core/Engine.h"
#include "Frame/TFrame.h"
#include "Frame/FrameProcessor.h"
#include "Frame/Operator/JRToneMapping.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Common/logging.h"
#include "Common/config.h"
#include "Math/Geometry/TAABB2D.h"
#include "Utility/Timer.h"
#include "EngineEnv/Session/RenderSession.h"
#include "EngineEnv/CoreCookingContext.h"
#include "Core/Receiver/Receiver.h"
#include "Core/Renderer/Renderer.h"
#include "Math/Transform/RigidTransform.h"
#include "Common/stats.h"

#include <fstream>
#include <string>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(Engine, Core);
PH_DEFINE_INTERNAL_TIMER_STAT(Update, Engine);
PH_DEFINE_INTERNAL_TIMER_STAT(Render, Engine);
PH_DEFINE_INTERNAL_TIMER_STAT(LoadCommands, Engine);

Engine::Engine() : 
	m_cooked(),
	m_visualWorld()
{
	setNumRenderThreads(1);

	PH_LOG(Engine, "Photon version: {}, PSDL version: {}",
		PH_ENGINE_VERSION, PH_PSDL_VERSION);
}

void Engine::enterCommand(const std::string& commandFragment)
{
	m_parser.enter(commandFragment, m_rawScene);
}

bool Engine::loadCommands(const Path& filePath)
{
	PH_SCOPED_TIMER(LoadCommands);

	std::ifstream commandFile;
	commandFile.open(filePath.toAbsoluteString(), std::ios::in);
	if(!commandFile.is_open())
	{
		PH_LOG_WARNING(Engine, "command file <{}> opening failed", 
			filePath.toAbsoluteString());

		return false;
	}
	else
	{
		PH_LOG(Engine, "loading command file <{}>", filePath.toAbsoluteString());

		Timer timer;
		timer.start();

		std::string lineCommand;
		while(commandFile.good())
		{
			std::getline(commandFile, lineCommand);
			lineCommand += '\n';

			enterCommand(lineCommand);
		}

		timer.stop();

		PH_LOG(Engine, "command file PSDL version: {}", m_parser.getCommandVersion().toString());
		PH_LOG(Engine, "command file loaded, time elapsed = {} ms", timer.getDeltaMs());

		return true;
	}
}

void Engine::update()
{
	PH_SCOPED_TIMER(Update);

	// Wait all potentially unfinished commands
	m_parser.flush(m_rawScene);

	PH_LOG(Engine, "parsed {} commands, {} errors generated", 
		m_parser.numParsedCommands(), m_parser.numParseErrors());

	std::shared_ptr<RenderSession> renderSession;
	{
		// TODO: means to specify the requested session

		std::vector<std::shared_ptr<RenderSession>> renderSessions = m_rawScene.getResources().getAll<RenderSession>();
		if(renderSessions.empty())
		{
			PH_LOG_ERROR(Engine, "require at least a render session; engine failed to update");

			return;
		}

		if(renderSessions.size() > 1)
		{
			PH_LOG_WARNING(Engine, "more than 1 render session specified, taking the last one");
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

	Renderer* const renderer = m_cooked.getRenderer();
	if(!renderer)
	{
		PH_LOG_ERROR(Engine, "no renderer present");

		return;
	}

	// TODO: not using hacky way to get receiver position
	PH_ASSERT(m_cooked.getReceiver());
	math::Vector3R receiverPos;
	m_cooked.getReceiver()->getReceiverToWorld().transformP({0, 0, 0}, &receiverPos);
	m_visualWorld.setReceiverPosition(receiverPos);

	m_visualWorld.cook(m_rawScene, coreCtx);

	// Update renderer

	PH_ASSERT(renderer);

	if(m_numRenderThreads != renderer->numWorkers())
	{
		PH_LOG(Engine, "overriding # render workers to {} (it was {})", 
			m_numRenderThreads, renderer->numWorkers());

		renderer->setNumWorkers(m_numRenderThreads);
	}

	getRenderer()->update(m_cooked, m_visualWorld);
}

void Engine::render()
{
	PH_SCOPED_TIMER(Render);

	Renderer* const renderer = getRenderer();
	if(!renderer)
	{
		PH_LOG_ERROR(Engine, "no renderer present");

		return;
	}

	PH_ASSERT(renderer);

	renderer->render();
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
	m_numRenderThreads = numThreads;

	PH_LOG(Engine, "number of render threads set to {}", numThreads);
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
