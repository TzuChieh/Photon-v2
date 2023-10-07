#include "Render/Renderer/OfflineRenderer.h"
#include "Designer/Render/RenderConfig.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Core/Engine.h>
#include <Core/Renderer/Renderer.h>
#include <Utility/TFunction.h>
#include <DataIO/io_utils.h>
#include <Frame/TFrame.h>
#include <Utility/exception.h>

#include <memory>
#include <thread>
#include <stop_token>

namespace ph::editor::render
{

PH_DEFINE_INTERNAL_LOG_GROUP(OfflineRenderer, Editor);

OfflineRenderer::OfflineRenderer()

	: SceneRenderer()

	, m_engineThread()
	, m_renderStage(EOfflineRenderStage::Finished)
	, m_syncedRenderStats()
	, m_requestRenderStats()
{}

OfflineRenderer::~OfflineRenderer()
{}

void OfflineRenderer::render(const RenderConfig& config)
{
	if(getRenderStage() != EOfflineRenderStage::Finished)
	{
		PH_LOG_WARNING(OfflineRenderer,
			"Please wait until current rendering task is finished before starting another render.");
		return;
	}

	setRenderStage(EOfflineRenderStage::Standby);

	if(!m_engineThread.hasStarted())
	{
		m_engineThread.setWorkProcessor(
			[](const EngineWork& work)
			{
				work();
			});

		m_engineThread.start();
	}

	m_engineThread.addWork(
		[this, config]()
		{
			try
			{
				renderSingleStaticImageOnEngineThread(config);
			}
			catch(const Exception& e)
			{
				PH_LOG_ERROR(OfflineRenderer,
					"Error on rendering single static image: {}", e.what());
			}
			
			setRenderStage(EOfflineRenderStage::Finished);
		});
}

bool OfflineRenderer::tryGetRenderStats(OfflineRenderStats* stats)
{
	// Always make a request
	m_requestRenderStats.test_and_set(std::memory_order_relaxed);
	m_requestRenderStats.notify_one();

	if(!stats)
	{
		return false;
	}

	if(auto locked = m_syncedRenderStats.tryConstLock())
	{
		*stats = *locked;
		return true;
	}
	else
	{
		return false;
	}
}

void OfflineRenderer::renderSingleStaticImageOnEngineThread(const RenderConfig& config)
{
	if(config.useCopiedScene)
	{
		setRenderStage(EOfflineRenderStage::CopyingScene);

		// TODO: copy scene
	}

	setRenderStage(EOfflineRenderStage::LoadingScene);

	auto renderEngine = std::make_unique<Engine>();
	renderEngine->setWorkingDirectory(config.sceneWorkingDirectory);
	renderEngine->loadCommands(config.sceneFile);

	setRenderStage(EOfflineRenderStage::Updating);

	renderEngine->update();

	Renderer* renderer = renderEngine->getRenderer();
	if(!renderer)
	{
		PH_LOG_ERROR(OfflineRenderer,
			"Failed to retrieve renderer from engine. Cannot render single static image.");
		return;
	}

	std::jthread statsRequestThread;
	if(config.enableStatsRequest)
	{
		ObservableRenderData entries = renderer->getObservableData();
		Viewport viewport = renderer->getViewport();

		// Load stats that are constant throughout the rendering process once
		m_syncedRenderStats.locked(
			[&viewport , &entries](OfflineRenderStats& stats)
			{
				stats.viewport = viewport;

				stats.layerNames.clear();
				for(std::size_t i = 0; i < entries.numLayers(); ++i)
				{
					stats.layerNames.push_back(entries.getLayerName(i));
				}
				
				stats.numericInfos.clear();
				for(std::size_t i = 0; i < entries.numIntegerStates(); ++i)
				{
					stats.numericInfos.push_back({
						.name = entries.getIntegerStateName(i),
						.isInteger = true});
				}
				for(std::size_t i = 0; i < entries.numRealStates(); ++i)
				{
					stats.numericInfos.push_back({
						.name = entries.getRealStateName(i),
						.isInteger = false});
				}
			});

		// Respond to stats request
		statsRequestThread = std::jthread(
			[this, &renderer](std::stop_token token)
			{
				while(!token.stop_requested())
				{
					m_requestRenderStats.wait(false, std::memory_order_relaxed);

					if(auto locked = m_syncedRenderStats.tryLock())
					{
						RenderStats stats = renderer->asyncQueryRenderStats();
						
						std::size_t intIdx = 0;
						std::size_t realIdx = 0;
						for(auto& info : locked->numericInfos)
						{
							if(info.isInteger)
							{
								info.value = static_cast<float64>(stats.getInteger(intIdx));
								++intIdx;
							}
							else
							{
								info.value = stats.getReal(realIdx);
								++realIdx;
							}
						}
					}

					m_requestRenderStats.clear(std::memory_order_relaxed);
				}
			});
	}

	// TODO: respond to peek frame request

	setRenderStage(EOfflineRenderStage::Rendering);

	renderEngine->render();

	if(statsRequestThread.joinable())
	{
		statsRequestThread.request_stop();
		statsRequestThread.join();
	}

	// TODO: stop the peek frame request

	setRenderStage(EOfflineRenderStage::Developing);

	// TODO: get final frame

	// Save result to disk
	HdrRgbFrame frame(renderer->getRenderWidthPx(), renderer->getRenderHeightPx());
	renderer->retrieveFrame(0, frame);
	io_utils::save(frame, config.outputDirectory, config.outputName, config.outputFileFormat);
}

void OfflineRenderer::setupGHI(GHIThreadCaller& caller)
{
	// TODO
}

void OfflineRenderer::cleanupGHI(GHIThreadCaller& caller)
{
	// TODO
}

void OfflineRenderer::update(const RenderThreadUpdateContext& ctx)
{
	// TODO
}

void OfflineRenderer::createGHICommands(GHIThreadCaller& caller)
{
	// TODO
}

}// end namespace ph::editor::render
