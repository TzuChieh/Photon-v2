#pragma once

#include "Core/Renderer/Sampling/SamplingRenderer.h"
#include "Core/Renderer/Region/Region.h"
#include "Common/primitive_type.h"
#include "Core/Renderer/Region/DammertzDispatcher.h"
#include "Core/Renderer/Region/GridScheduler.h"
#include "Core/Renderer/Sampling/CameraSamplingWork.h"
#include "Frame/TFrame.h"
#include "Core/Renderer/Sampling/TStepperCameraMeasurementEstimator.h"
#include "Core/Renderer/Sampling/MetaRecordingProcessor.h"
#include "Core/Quantity/SpectralStrength.h"

#include <memory>
#include <queue>
#include <cstddef>
#include <vector>
#include <functional>

namespace ph
{

class FixedSizeThreadPool;

class AdaptiveSamplingRenderer : public SamplingRenderer, public TCommandInterface<AdaptiveSamplingRenderer>
{
public:
	void doUpdate(const SdlResourcePack& data) override;
	void doRender() override;
	void retrieveFrame(std::size_t layerIndex, HdrRgbFrame& out_frame) override;

	ERegionStatus asyncPollUpdatedRegion(Region* out_region) override;
	RenderState asyncQueryRenderState() override;
	RenderProgress asyncQueryRenderProgress() override;
	void asyncPeekFrame(
		std::size_t   layerIndex,
		const Region& region,
		HdrRgbFrame&  out_frame) override;

	ObservableRenderData getObservableData() const override;

private:
	using FilmEstimator = TStepperCameraMeasurementEstimator<HdrRgbFilm, SpectralStrength>;

	constexpr static auto REFINE_MODE = DammertzDispatcher::ERefineMode::MIN_ERROR_DIFFERENCE;
	//constexpr static auto REFINE_MODE = DammertzDispatcher::ERefineMode::MIDPOINT;

	const Scene*               m_scene;
	const Camera*              m_camera;
	SampleGenerator*           m_sampleGenerator;
	HdrRgbFilm                 m_allEffortFilm;
	HdrRgbFilm                 m_halfEffortFilm;

	std::vector<CameraSamplingWork>         m_renderWorks;
	std::vector<FilmEstimator>              m_filmEstimators;

	std::vector<MetaRecordingProcessor> m_metaRecorders;
	HdrRgbFrame m_metaFrame;

	DammertzDispatcher                    m_dispatcher;
	std::vector<uint32>                   m_freeWorkerIds;
	real                                  m_precisionStandard;
	std::size_t                           m_numInitialSamples;
	HdrRgbFrame                           m_allEffortFrame;
	HdrRgbFrame                           m_halfEffortFrame;

	struct UpdatedRegion
	{
		Region region;
		bool   isFinished;
	};
	std::deque<UpdatedRegion> m_updatedRegions;

	std::mutex           m_rendererMutex;
	std::atomic_uint64_t m_totalPaths;
	std::atomic_uint32_t m_suppliedFractionBits;
	std::atomic_uint32_t m_submittedFractionBits;
	std::atomic_uint32_t m_numNoisyRegions;

	void addUpdatedRegion(const Region& region, bool isUpdating);

	std::function<void()> createWork(FixedSizeThreadPool& workers, uint32 workerId);

// command interface
public:
	explicit AdaptiveSamplingRenderer(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  renderer          </category>
	<type_name> adaptive-sampling </type_name>
	<extend>    renderer.sampling </extend>

	<name> Adaptive Sampling Renderer </name>
	<description>
		This renderer renders images by path sampling techniques, but the 
		samples will be concentrated on noisy regions. Normally, this renderer
		has better utilization of computational power.
	</description>

	<command type="creator">
	</command>

	</SDL_interface>
*/
