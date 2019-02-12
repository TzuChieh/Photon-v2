#pragma once

#include "Core/Renderer/Sampling/SamplingRenderer.h"
#include "Core/Renderer/Region/Region.h"
#include "Common/primitive_type.h"
#include "Core/Renderer/Region/DammertzDispatcher.h"
#include "Core/Renderer/Region/GridScheduler.h"
#include "Core/Renderer/Sampling/CameraSamplingWork.h"
#include "Frame/TFrame.h"
#include "Core/Renderer/Sampling/StepperFilmEnergyEstimator.h"

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
	void develop(HdrRgbFrame& out_frame, EAttribute attribute) override;

	ERegionStatus asyncPollUpdatedRegion(Region* out_region) override;
	RenderState asyncQueryRenderState() override;
	RenderProgress asyncQueryRenderProgress() override;
	void asyncPeekRegion(HdrRgbFrame& out_frame, const Region& region, EAttribute attribute) override;

	ObservableRenderData getObservableData() const override;

private:
	constexpr static auto REFINE_MODE = DammertzDispatcher::ERefineMode::MIN_ERROR_DIFFERENCE;

	const Scene*               m_scene;
	const Camera*              m_camera;
	SampleGenerator*           m_sampleGenerator;
	SampleFilter               m_filter;
	HdrRgbFilm                 m_allEffortFilm;
	HdrRgbFilm                 m_halfEffortFilm;

	std::unique_ptr<FullRayEnergyEstimator> m_estimator;
	std::vector<CameraSamplingWork>         m_renderWorks;
	std::vector<StepperFilmEnergyEstimator> m_filmEstimators;

	DammertzDispatcher                    m_dispatcher;
	std::vector<uint32>                   m_freeWorkerIds;
	real                                  m_precisionStandard;
	std::size_t                           m_numPathsPerRegion;
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

	void addUpdatedRegion(const Region& region, bool isUpdating);

	std::function<void()> createWork(FixedSizeThreadPool& workers, uint32 workerId);

// command interface
public:
	explicit AdaptiveSamplingRenderer(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph
