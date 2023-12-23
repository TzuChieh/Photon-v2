#pragma once

#include "Core/Renderer/Renderer.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Filmic/SampleFilter.h"
#include "Core/Renderer/PM/EPMMode.h"
#include "Core/Renderer/PM/PMAtomicStatistics.h"
#include "Core/Renderer/PM/PMCommonParams.h"

#include <vector>
#include <memory>
#include <atomic>
#include <cstddef>
#include <mutex>

namespace ph
{

class Receiver;

class PMRenderer : public Renderer
{
public:
	PMRenderer(
		EPMMode mode,
		PMCommonParams commonParams,
		Viewport viewport,
		SampleFilter filter,
		uint32 numWorkers);

	void doUpdate(const CoreCookedUnit& cooked, const VisualWorld& world) override;
	void doRender() override;
	void retrieveFrame(std::size_t layerIndex, HdrRgbFrame& out_frame) override;

	std::size_t asyncPollUpdatedRegions(TSpan<RenderRegionStatus> out_regions) override;
	RenderStats asyncQueryRenderStats() override;
	RenderProgress asyncQueryRenderProgress() override;

	void asyncPeekFrame(
		std::size_t   layerIndex,
		const Region& region,
		HdrRgbFrame&  out_frame) override;

	RenderObservationInfo getObservationInfo() const override;

	void asyncMergeFilm(const HdrRgbFilm& srcFilm);
	void asyncReplaceFilm(const HdrRgbFilm& srcFilm);

private:
	std::unique_ptr<HdrRgbFilm> m_film;

	const Scene* m_scene;
	const Receiver* m_receiver;
	SampleGenerator* m_sg;
	SampleFilter m_filter;

	EPMMode m_mode;
	PMCommonParams m_commonParams;

	std::mutex m_filmMutex;

	PMAtomicStatistics m_statistics;
	std::atomic_uint32_t m_photonsPerSecond;
	std::atomic_bool m_isFilmUpdated;

	void renderWithProgressivePM();
	void renderWithStochasticProgressivePM();
};

}// end namespace ph
