#include "Render/Renderer/OfflineRenderer.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Core/Engine.h>
#include <Core/Renderer/Renderer.h>
#include <Utility/TFunction.h>

#include <memory>
#include <thread>
#include <stop_token>

namespace ph::editor::render
{

PH_DEFINE_INTERNAL_LOG_GROUP(OfflineRenderer, Editor);

OfflineRenderer::OfflineRenderer()

	: SceneRenderer()

	, m_engineThread()
	, m_syncedRenderStats()
	, m_requestRenderStats()
{}

OfflineRenderer::~OfflineRenderer()
{}

void OfflineRenderer::render(const OfflineRenderConfig& config)
{
	if(!m_engineThread.hasStarted())
	{
		m_engineThread.start();
	}

	m_engineThread.addWork(
		[this, config]()
		{
			renderSingleStaticImageOnEngineThread(config);
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

void OfflineRenderer::renderSingleStaticImageOnEngineThread(const OfflineRenderConfig& config)
{
	auto renderEngine = std::make_unique<Engine>();
	renderEngine->setWorkingDirectory(config.sceneWorkingDirectory);
	renderEngine->loadCommands(config.sceneFile);
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
		m_syncedRenderStats.locked(
			[&entries](OfflineRenderStats& stats)
			{
				for(std::size_t i = 0; i < entries.numLayers(); ++i)
				{
					stats.layerNames.push_back(entries.getLayerName(i));
				}
				
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

	renderEngine->render();

	if(statsRequestThread.joinable())
	{
		statsRequestThread.request_stop();
		statsRequestThread.join();
	}

	// TODO: stop the peek frame request

	// TODO: get final frame or save file
}

void OfflineRenderer::setupGHI(GHIThreadCaller& caller)
{
	// TODO
}

void OfflineRenderer::cleanupGHI(GHIThreadCaller& caller)
{
	// TODO
}

void OfflineRenderer::update(const UpdateContext& ctx)
{
	// TODO
}

void OfflineRenderer::createGHICommands(GHIThreadCaller& caller)
{
	// TODO
}

}// end namespace ph::editor::render
