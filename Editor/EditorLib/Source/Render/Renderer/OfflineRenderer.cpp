#include "Render/Renderer/OfflineRenderer.h"
#include "Designer/Render/RenderConfig.h"
#include "RenderCore/GHIThreadCaller.h"
#include "RenderCore/GraphicsContext.h"
#include "RenderCore/GraphicsMemoryManager.h"
#include "RenderCore/GHI.h"
#include "RenderCore/Memory/GraphicsArena.h"
#include "RenderCore/ghi_enums.h"
#include "EditorCore/Thread/Threads.h"

#include <Common/assertion.h>
#include <Common/exception.h>
#include <Common/logging.h>
#include <Common/profiling.h>
#include <Core/Engine.h>
#include <Core/Renderer/Renderer.h>
#include <Core/Renderer/RenderRegionStatus.h>
#include <Utility/TFunction.h>
#include <DataIO/io_utils.h>
#include <Frame/TFrame.h>
#include <Core/Renderer/RenderProgress.h>
#include <Core/Renderer/RenderStats.h>
#include <Frame/Operator/JRToneMapping.h>
#include <DataIO/FileSystem/Filesystem.h>

#include <memory>
#include <stop_token>
#include <unordered_set>
#include <chrono>
#include <vector>

namespace ph::editor::render
{

PH_DEFINE_INTERNAL_LOG_GROUP(OfflineRenderer, Editor);

OfflineRenderer::OfflineRenderer()

	: SceneRenderer()

	, m_engineThread()
	, m_renderStage(EOfflineRenderStage::Finished)
	, m_syncedRenderStats()
	, m_syncedRenderPeek()
	, m_cachedRenderPeekInput()
	, m_synchedFrameData()
	, m_requestCompletedFrameDataUpload()
	, m_requestRenderStats()
	, m_requestRenderPeek()
{}

OfflineRenderer::~OfflineRenderer()
{}

void OfflineRenderer::render(RenderConfig config)
{
	PH_ASSERT(Threads::isOnRenderThread());

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

void OfflineRenderer::renderSingleStaticImageOnEngineThread(RenderConfig config)
{
	if(config.useCopiedScene)
	{
		setRenderStage(EOfflineRenderStage::CopyingScene);

		Path dstDir = config.sceneWorkingDirectory / "_Intermediate";
		Filesystem::copyFileToDirectory(config.sceneFile, dstDir, true);
		
		// Set scene file to the copied one. Scene working directory should not be changed as
		// asset files are not copied and expect to stay the same during rendering.
		config.sceneFile = dstDir / config.sceneFile.getFilename();
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

	Timer renderTimer;
	renderTimer.start();

	renderEngine->render();

	// No need to monitor the render engine once `Renderer::render()` returns
	statsRequestThread.request_stop();
	peekFrameThread.request_stop();

	setRenderStage(EOfflineRenderStage::Developing);

	// Get completed frame
	HdrRgbFrame frame(renderer->getRenderWidthPx(), renderer->getRenderHeightPx());
	math::TAABB2D<uint32> fullRegion({0, 0}, frame.getSizePx());
	renderer->retrieveFrame(0, frame);
	if(config.performToneMapping)
	{
		JRToneMapping{}.operateLocal(frame, fullRegion);
	}

	if(config.enablePeekingFrame)
	{
		m_synchedFrameData->frame = frame;
		m_synchedFrameData->updatedRegion = math::TAABB2D<int32>(fullRegion);
		m_requestCompletedFrameDataUpload.test_and_set(std::memory_order_relaxed);
	}

	// Save result to disk
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
{}

void OfflineRenderer::cleanupGHI(GHIThreadCaller& caller)
{}

void OfflineRenderer::update(const RenderThreadUpdateContext& ctx)
{
	PH_PROFILE_SCOPE();

	if(getRenderStage() == EOfflineRenderStage::Rendering)
	{
		if(auto locked = m_syncedRenderPeek.tryConstLock())
		{
			m_cachedRenderPeekInput = locked->in;
		}
	}
}

void OfflineRenderer::createGHICommands(GHIThreadCaller& caller)
{
	PH_PROFILE_SCOPE();

	// Respond to the data upload request for peeking intermediate frame during rendering.
	if(getRenderStage() == EOfflineRenderStage::Rendering &&
	   m_cachedRenderPeekInput.wantIntermediateResult)
	{
		tryUploadFrameData(caller);
	}

	// Respond to the data upload request for a completed frame. Frame data is synchronized, and the
	// flag is set in a way that guarantees frame data visibility. However, if any subsequent frame
	// is completed too soon or the try-upload operation is failing for long enough time (both are
	// very unlikely, but can happen), the newer requests can be missed. Acceptable as this is for
	// preview only.
	if(m_requestCompletedFrameDataUpload.test(std::memory_order_relaxed))
	{
		if(tryUploadFrameData(caller))
		{
			m_requestCompletedFrameDataUpload.clear(std::memory_order_relaxed);
		}
	}
}

bool OfflineRenderer::tryUploadFrameData(GHIThreadCaller& caller)
{
	PH_PROFILE_NAMED_SCOPE("Copy updated region to arena");
	PH_ASSERT(Threads::isOnRenderThread());

	if(auto locked = m_synchedFrameData.tryConstLock();
	   locked && !locked->updatedRegion.isEmpty())
	{
		math::Vector2UI regionOrigin(locked->updatedRegion.getMinVertex());
		math::Vector2UI regionSize(locked->updatedRegion.getExtents());

		ghi::GraphicsArena arena = caller.getGraphicsContext().getMemoryManager().newRenderProducerHostArena();
		auto regionData = arena.makeArray<HdrComponent>(std::size_t(3) * regionSize.x() * regionSize.y());
		locked->frame.copyPixelData(math::TAABB2D<uint32>(locked->updatedRegion), regionData);

		caller.add(
			[regionData, regionOrigin, regionSize, handle = m_cachedRenderPeekInput.resultHandle]
			(ghi::GraphicsContext& ctx)
			{
				PH_PROFILE_NAMED_SCOPE("Upload updated region");

				ctx.getGHI().tryUploadPixelDataTo2DRegion(
					handle, 
					regionOrigin, 
					regionSize,
					std::as_bytes(regionData),
					ghi::EPixelFormat::RGB,
					ghi::EPixelComponent::Float32);
			});

		return true;
	}
	
	return false;
}

std::jthread OfflineRenderer::makeStatsRequestThread(Renderer* renderer, uint32 minPeriodMs)
{
	return std::jthread([this, renderer, minPeriodMs](std::stop_token token)
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
				locked->renderTimeMs = progress.getElapsedMs();

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
	return std::jthread([this, renderer, minPeriodMs](std::stop_token token)
	{
		// We need to decide how many regions to poll in one peek request. Too small, we might
		// never catch up with the speed of newly added regions. The number of concurrent CPU
		// threads is a nice value to multiply from as the rendering speed should be roughly
		// proportional to it.
		std::vector<RenderRegionStatus> regionPollBuffer(std::thread::hardware_concurrency() * 4);

		std::unordered_set<math::TAABB2D<int32>> uniqueUpdatingRegions;
		math::TAABB2D<int64> updatedRegion = math::TAABB2D<int64>::makeEmpty();
		OfflineRenderPeek::Input cachedInput;

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

			if(cachedInput.wantUpdatingRegions || cachedInput.wantIntermediateResult)
			{
				const auto numNewRegions = renderer->asyncPollUpdatedRegions(regionPollBuffer);
					
				// Grow the buffer exponentially so we can always catch up
				if(regionPollBuffer.size() == numNewRegions)
				{
					// Effective growth rate k = 1.5
					const auto newBufferSize = regionPollBuffer.size() + regionPollBuffer.size() / 2;
					regionPollBuffer.resize(newBufferSize);
				}
					
				for(uint32 ri = 0; ri < numNewRegions; ++ri)
				{
					const Region region = regionPollBuffer[ri].getRegion();
					const ERegionStatus status = regionPollBuffer[ri].getStatus();

					// Skip uninteresting region statuses
					if(status == ERegionStatus::Invalid)
					{
						continue;
					}

					updatedRegion.unionWith(region);

					if(cachedInput.wantUpdatingRegions)
					{
						const math::TAABB2D<int32> castedRegion(region);
						if(status == ERegionStatus::Updating)
						{
							uniqueUpdatingRegions.insert(castedRegion);
						}
						else if(status == ERegionStatus::Finished)
						{
							uniqueUpdatingRegions.erase(castedRegion);
						}
					}
				}
			}

			if(cachedInput.wantIntermediateResult && !updatedRegion.isEmpty())
			{
				if(auto locked = m_synchedFrameData.tryLock())
				{
					locked->frame.setSize(renderer->getViewport().getBaseSizePx());
					renderer->asyncPeekFrame(cachedInput.layerIndex, updatedRegion, locked->frame);
					if(cachedInput.performToneMapping)
					{
						JRToneMapping{}.operateLocal(locked->frame, math::TAABB2D<uint32>(updatedRegion));
					}

					// Append the new region to output
					locked->updatedRegion.unionWith(math::TAABB2D<int32>(updatedRegion));
					updatedRegion = math::TAABB2D<int64>::makeEmpty();
				}
			}

			if(auto locked = m_syncedRenderPeek.tryLock())
			{
				if(cachedInput.wantUpdatingRegions)
				{
					locked->out.updatingRegions.clear();
					for(const math::TAABB2D<int32>& region : uniqueUpdatingRegions)
					{
						locked->out.updatingRegions.push_back(region);
					}
				}

				cachedInput = locked->in;
			}
		}
	});
}

}// end namespace ph::editor::render
