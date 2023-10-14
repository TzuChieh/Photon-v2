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
#include <Core/Renderer/RenderProgress.h>
#include <Core/Renderer/RenderStats.h>

#include <memory>
#include <stop_token>
#include <unordered_set>
#include <chrono>

namespace ph::editor::render
{

PH_DEFINE_INTERNAL_LOG_GROUP(OfflineRenderer, Editor);

OfflineRenderer::OfflineRenderer()

	: SceneRenderer()

	, m_engineThread()
	, m_renderStage(EOfflineRenderStage::Finished)
	, m_syncedRenderStats()
	, m_syncedRenderPeek()
	, m_requestRenderStats()
	, m_requestRenderPeek()
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

bool OfflineRenderer::tryGetRenderPeek(OfflineRenderPeek* peek, bool shouldUpdateInput)
{
	// Always make a request
	m_requestRenderPeek.test_and_set(std::memory_order_relaxed);
	m_requestRenderPeek.notify_one();

	if(!peek)
	{
		return false;
	}

	if(shouldUpdateInput)
	{
		if(auto locked = m_syncedRenderPeek.tryLock())
		{
			locked->in = peek->in;
			peek->out = locked->out;
			return true;
		}
	}
	else
	{
		if(auto locked = m_syncedRenderPeek.tryConstLock())
		{
			peek->out = locked->out;
			return true;
		}
	}

	return false;
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
		RenderObservationInfo entries = renderer->getObservationInfo();
		Viewport viewport = renderer->getViewport();

		// Load stats that are constant throughout the rendering process once
		m_syncedRenderStats.locked(
			[&viewport , &entries](OfflineRenderStats& stats)
			{
				stats = OfflineRenderStats{};
				stats.viewport = viewport;

				for(std::size_t i = 0; i < entries.numLayers(); ++i)
				{
					stats.layerNames.push_back(entries.getLayerName(i));
				}
				
				for(std::size_t i = 0; i < entries.numIntegerStats(); ++i)
				{
					stats.numericInfos.push_back({
						.name = entries.getIntegerStatName(i),
						.isInteger = true});
				}

				for(std::size_t i = 0; i < entries.numRealStats(); ++i)
				{
					stats.numericInfos.push_back({
						.name = entries.getRealStatName(i),
						.isInteger = false});
				}
			});

		// Respond to stats request
		statsRequestThread = makeStatsRequestThread(renderer, config.minStatsRequestPeriodMs);
	}

	std::jthread peekFrameThread;
	if(config.enablePeekingFrame)
	{
		// Respond to peek request
		peekFrameThread = makePeekFrameThread(renderer, config.minFramePeekPeriodMs);
	}

	setRenderStage(EOfflineRenderStage::Rendering);

	renderEngine->render();

	// No need to monitor the render engine once `Renderer::render()` returns
	statsRequestThread.request_stop();
	peekFrameThread.request_stop();

	setRenderStage(EOfflineRenderStage::Developing);

	// TODO: get final frame

	// Save result to disk
	HdrRgbFrame frame(renderer->getRenderWidthPx(), renderer->getRenderHeightPx());
	renderer->retrieveFrame(0, frame);
	io_utils::save(frame, config.outputDirectory, config.outputName, config.outputFileFormat);

	if(statsRequestThread.joinable())
	{
		statsRequestThread.join();
	}

	if(peekFrameThread.joinable())
	{
		peekFrameThread.join();
	}
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

std::jthread OfflineRenderer::makeStatsRequestThread(Renderer* renderer, uint32 minPeriodMs)
{
	return std::jthread(
		[this, renderer, minPeriodMs](std::stop_token token)
		{
			while(!token.stop_requested())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(minPeriodMs));
				if(m_requestRenderStats.test(std::memory_order_relaxed))
				{
					m_requestRenderStats.clear(std::memory_order_relaxed);
				}
				else
				{
					continue;
				}

				if(auto locked = m_syncedRenderStats.tryLock())
				{
					RenderProgress progress = renderer->asyncQueryRenderProgress();
					RenderStats stats = renderer->asyncQueryRenderStats();

					locked->totalWork = progress.getTotalWork();
					locked->workDone = progress.getWorkDone();

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
				}// end perform request
			}
		});
}

std::jthread OfflineRenderer::makePeekFrameThread(Renderer* renderer, uint32 minPeriodMs)
{
	return std::jthread(
		[this, renderer, minPeriodMs](std::stop_token token)
		{
			std::unordered_set<math::TAABB2D<int32>> uniqueRegions;
			bool wantRegions = false;
			bool wantFrame = false;

			// We need to decide how many regions to poll in one peek request. Too small, we might
			// never catch up with the speed of newly added regions. The number of concurrent CPU
			// threads is a nice value to multiply from as the rendering speed should be roughly
			// proportional to it.
			const auto numRegionPollsAtOnce = std::thread::hardware_concurrency() * 4;

			while(!token.stop_requested())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(minPeriodMs));
				if(m_requestRenderPeek.test(std::memory_order_relaxed))
				{
					m_requestRenderPeek.clear(std::memory_order_relaxed);
				}
				else
				{
					continue;
				}

				if(wantRegions)
				{
					for(uint32 i = 0; i < numRegionPollsAtOnce; ++i)
					{
						Region region;
						ERegionStatus status = renderer->asyncPollUpdatedRegion(&region);
						if(status == ERegionStatus::INVALID)
						{
							break;
						}

						math::TAABB2D<int32> castedRegion(region);
						if(status == ERegionStatus::UPDATING)
						{
							uniqueRegions.insert(castedRegion);
						}
						else if(status == ERegionStatus::FINISHED)
						{
							uniqueRegions.erase(castedRegion);
						}
					}
				}

				if(auto locked = m_syncedRenderPeek.tryLock())
				{
					if(wantRegions)
					{
						locked->out.updatingRegions.clear();
						for(const math::TAABB2D<int32>& uniqueRegion : uniqueRegions)
						{
							locked->out.updatingRegions.push_back(uniqueRegion);
						}
					}

					wantFrame = locked->in.wantIntermediateResult;
					wantRegions = locked->in.wantUpdatingRegions;
				}
			}
		});
}

}// end namespace ph::editor::render
