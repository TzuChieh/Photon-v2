#pragma once

#include "Engine/Core/Renderer/Sampling/SamplingRenderer.h"
#include "Engine/Core/Scheduler/Region.h"
#include "Engine/Core/Scheduler/DammertzDispatcher.h"
#include "Engine/Core/Filmic/HdrRgbFilm.h"
#include "Engine/Core/Renderer/Sampling/ReceiverSamplingWork.h"
#include "Engine/Frame/TFrame.h"
#include "Engine/Core/Renderer/Sampling/TStepperReceiverMeasurementProcessor.h"
#include "Engine/Core/Renderer/Sampling/MetaRecordingProcessor.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Utility/Concurrent/TAtomicQuasiQueue.h"

#include <Common/primitive_type.h>

#include <memory>
#include <queue>
#include <cstddef>
#include <vector>
#include <functional>

namespace ph
{

class Scene;
class Receiver;
class FixedSizeThreadPool;

class AdaptiveSamplingRenderer : public SamplingRenderer
{
public:
	void doUpdate(const CoreCookedUnit& cooked, const VisualWorld& world) override;
	void doRender() override;
	void retrieveFrame(int32 layerIndex, HdrRgbFrame& out_frame) override;

	std::size_t asyncPollUpdatedRegions(TSpan<RenderRegionStatus> out_regions) override;
	RenderStats asyncQueryRenderStats() override;
	RenderProgress asyncQueryRenderProgress() override;
	void asyncPeekFrame(
		int32 layerIndex,
		const Region& region,
		HdrRgbFrame&  out_frame) override;

	RenderObservationInfo getObservationInfo() const override;

private:
	using RayProcessor = TStepperReceiverMeasurementProcessor<math::Spectrum>;

	constexpr static auto REFINE_MODE = DammertzDispatcher::ERefineMode::MIN_ERROR_DIFFERENCE;
	//constexpr static auto REFINE_MODE = DammertzDispatcher::ERefineMode::MIDPOINT;

	void asyncAddUpdatedRegion(const Region& region, bool isUpdating);
	std::function<void()> createWork(FixedSizeThreadPool& workers, uint32 workerId);

	const Scene*               m_scene;
	const Receiver*            m_receiver;
	SampleGenerator*           m_sampleGenerator;
	HdrRgbFilm                 m_allEffortFilm;
	HdrRgbFilm                 m_halfEffortFilm;

	std::vector<ReceiverSamplingWork>      m_renderWorks;
	std::vector<RayProcessor>              m_rayProcessors;

	std::vector<MetaRecordingProcessor> m_metaRecorders;
	HdrRgbFrame m_metaFrame;

	DammertzDispatcher                    m_dispatcher;
	std::vector<uint32>                   m_freeWorkerIds;
	real                                  m_precisionStandard;
	std::size_t                           m_numInitialSamples;
	HdrRgbFrame                           m_allEffortFrame;
	HdrRgbFrame                           m_halfEffortFrame;
	TAtomicQuasiQueue<RenderRegionStatus> m_updatedRegionQueue;

	std::mutex           m_rendererMutex;
	std::atomic_uint64_t m_totalPaths;
	std::atomic_uint32_t m_suppliedFractionBits;
	std::atomic_uint32_t m_submittedFractionBits;
	std::atomic_uint32_t m_numNoisyRegions;
};

}// end namespace ph
