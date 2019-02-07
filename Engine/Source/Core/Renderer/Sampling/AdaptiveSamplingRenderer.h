#pragma once

#include "Core/Renderer/Sampling/SamplingRenderer.h"
#include "Core/Renderer/Region/Region.h"
#include "Common/primitive_type.h"
#include "Core/Renderer/Region/DammertzAdaptiveDispatcher.h"
#include "Core/Renderer/Region/GridScheduler.h"
#include "Core/Renderer/Sampling/SamplingRenderWork.h"
#include "Frame/TFrame.h"

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

	AttributeTags supportedAttributes() const override;
	std::string renderStateName(RenderState::EType type, std::size_t index) const override;

	// FIXME: these APIs are somewhat hacked and should be revisited
	void asyncUpdateFilm(HdrRgbFilm* workerFilm, bool isUpdating) override;
	void asyncDevelop(HdrRgbFrame& out_frame, EAttribute attribute);

private:
	const Scene*               m_scene;
	const Camera*              m_camera;
	SampleGenerator*           m_sampleGenerator;
	std::unique_ptr<Estimator> m_estimator;
	SampleFilter               m_filter;
	//SamplingFilmSet            m_films;
	AttributeTags              m_requestedAttributes;
	std::vector<SamplingRenderWork> m_renderWorks;

	DammertzAdaptiveDispatcher m_dispatcher;
	real m_precisionStandard;
	std::size_t m_numPathsPerRegion;
	GridScheduler m_currentGrid;
	HdrRgbFrame m_allEffortFrame;
	HdrRgbFrame m_halfEffortFrame;

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

	std::vector<uint32> m_stoppedWorkers;

	void clearWorkData();
	void mergeWorkFilms(HdrRgbFilm* workerFilms);
	void addUpdatedRegion(const Region& region, bool isUpdating);

	std::function<void()> createWork(FixedSizeThreadPool& workers, uint32 workerId);

// command interface
public:
	explicit AdaptiveSamplingRenderer(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph
