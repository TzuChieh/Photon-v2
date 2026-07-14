#pragma once

#include "Engine/Core/Renderer/Sampling/SamplingRenderer.h"
#include "Engine/Core/Filmic/SamplingFilmLayer.h"
#include "Engine/Core/Renderer/Sampling/ReceiverSamplingWork.h"
#include "Engine/Core/Renderer/Sampling/TReceiverMeasurementProcessor.h"
#include "Engine/Core/Scheduler/WorkScheduler.h"
#include "Engine/Core/Scheduler/EScheduler.h"
#include "Engine/Core/Renderer/Sampling/MetaRecordingProcessor.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Math/TVector2.h"
#include "Engine/Utility/Concurrent/TAtomicQuasiQueue.h"

#include <vector>
#include <memory>
#include <atomic>
#include <functional>
#include <string>

namespace ph
{

class Scene;
class Receiver;
class SampleGenerator;

class EqualSamplingRenderer : public SamplingRenderer
{
public:
	EqualSamplingRenderer(
		std::unique_ptr<IRayEnergyEstimator>           estimator,
		Viewport                                       viewport,
		SampleFilter                                   filter,
		uint32                                         numWorkers,
		EScheduler                                     scheduler,
		std::vector<SamplingFilmLayer<math::Spectrum>> filmLayers);

	void doUpdate(const CoreCookedUnit& cooked, const VisualWorld& world) override;
	void doRender() override;
	void retrieveFrame(uint32 layerIndex, HdrRgbFrame& out_frame) override;

	std::size_t asyncPollUpdatedRegions(TSpan<RenderRegionStatus> out_regions) override;
	RenderStats asyncQueryRenderStats() override;
	RenderProgress asyncQueryRenderProgress() override;

	void asyncPeekFrame(
		uint32        layerIndex,
		const Region& region,
		HdrRgbFrame&  out_frame) override;

	RenderObservableInfo getObservableInfo() const override;

private:
	using RayProcessor = TReceiverMeasurementProcessor<math::Spectrum>;

	void asyncAddUpdatedRegion(const Region& region, bool isUpdating);
	void initScheduler(std::size_t numSamplesPerPixel);

	const Scene*     m_scene;
	const Receiver*  m_receiver;
	SampleGenerator* m_sampleGenerator;
	
	std::unique_ptr<WorkScheduler>                 m_scheduler;
	EScheduler                                     m_schedulerType;
	std::vector<SamplingFilmLayer<math::Spectrum>> m_mainFilmLayers;
	math::Vector2S                                 m_blockSize;
	TAtomicQuasiQueue<RenderRegionStatus>          m_updatedRegionQueue;
	
	std::vector<ReceiverSamplingWork>   m_renderWorks;
	std::vector<RayProcessor>           m_rayProcessors;
	std::vector<MetaRecordingProcessor> m_metaRecorders;
	
	std::mutex           m_rendererMutex;
	std::atomic_uint64_t m_totalPaths;
	std::atomic_uint64_t m_totalElapsedMs;
	std::atomic_uint32_t m_suppliedFractionBits;
	std::atomic_uint32_t m_submittedFractionBits;
};

}// end namespace ph
