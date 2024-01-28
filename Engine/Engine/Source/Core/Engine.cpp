#include "Core/Engine.h"
#include "Frame/TFrame.h"
#include "Frame/FrameProcessor.h"
#include "Frame/Operator/JRToneMapping.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Math/Geometry/TAABB2D.h"
#include "EngineEnv/Session/RenderSession.h"
#include "EngineEnv/CoreCookingContext.h"
#include "Core/Receiver/Receiver.h"
#include "Core/Renderer/Renderer.h"
#include "Math/Transform/RigidTransform.h"
#include "SDL/sdl_exceptions.h"

#include <Common/config.h>
#include <Common/stats.h>
#include <Common/logging.h>

#include <fstream>
#include <string>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(Engine, Core);
PH_DEFINE_INTERNAL_TIMER_STAT(Update, Engine);
PH_DEFINE_INTERNAL_TIMER_STAT(Render, Engine);
PH_DEFINE_INTERNAL_TIMER_STAT(LoadCommands, Engine);

Engine::Engine() 
	: m_sceneParser()
	, m_rawScene()
	, m_visualWorld()
	, m_cooked()
	, m_numThreads(0)
{
	PH_LOG(Engine, Note,
		"Photon version: {}, PSDL version: {}",
		PH_ENGINE_VERSION, PH_PSDL_VERSION);

	m_sceneParser.setScene(&m_rawScene);
}

void Engine::enterCommand(const std::string& commandFragment)
{
	try
	{
		m_sceneParser.parse(commandFragment);
	}
	catch(const Exception& e)
	{
		PH_LOG(Engine, Note,
			"error running command: {}",
			e.what());
	}
}

bool Engine::loadCommands(const Path& sceneFile)
{
	PH_SCOPED_TIMER(LoadCommands);

	// Scene file may reside in a directory different to the scene working directory. Data files will
	// still be loaded from the scene working directory (call `setWorkingDirectory()` to set it).
	m_sceneParser.setSceneFile(sceneFile);

	try
	{
		m_sceneParser.read();
	}
	catch(const Exception& e)
	{
		PH_LOG(Engine, Note,
			"error loading scene file {}: {}", sceneFile.toAbsoluteString(), e.what());
		return false;
	}
	
	return true;
}

void Engine::update()
{
	PH_SCOPED_TIMER(Update);

	// Wait all potentially unfinished commands
	m_sceneParser.flush();

	PH_LOG(Engine, Note, "parsed {} commands, {} errors generated", 
		m_sceneParser.numParsedCommands(), m_sceneParser.numParseErrors());

	std::shared_ptr<RenderSession> renderSession;
	{
		// TODO: means to specify the requested session

		auto renderSessions = m_rawScene.getResources().getAllOfType<RenderSession>();
		if(renderSessions.empty())
		{
			PH_LOG(Engine, Error, "require at least a render session; engine failed to update");

			return;
		}

		if(renderSessions.size() > 1)
		{
			PH_LOG(Engine, Warning, "more than 1 render session specified, taking the last one");
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
		PH_LOG(Engine, Error, "no renderer present");

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

	if(m_numThreads != 0 && m_numThreads != renderer->numWorkers())
	{
		PH_LOG(Engine, Note,
			"Overriding # render workers to {} (it was {}).", 
			m_numThreads, renderer->numWorkers());

		renderer->setNumWorkers(m_numThreads);
	}

	getRenderer()->update(m_cooked, m_visualWorld);
}

void Engine::render()
{
	PH_SCOPED_TIMER(Render);

	Renderer* const renderer = getRenderer();
	if(!renderer)
	{
		PH_LOG(Engine, Error, "No renderer present. Render operation terminated.");
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

void Engine::setNumThreads(uint32 numThreads)
{
	m_numThreads = numThreads;

	PH_LOG(Engine, Note,
		"Number of threads set to {}{}.", 
		numThreads, numThreads > 0 ? "" : " (auto determine)");
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
	RenderStats stats = renderer->asyncQueryRenderStats();
	*out_percentageProgress = progress.getPercentageProgress();
	*out_samplesPerSecond = stats.getReal(0);
}

void Engine::setWorkingDirectory(const Path& directory)
{
	m_sceneParser.setSceneWorkingDirectory(directory);
}

}// end namespace ph
