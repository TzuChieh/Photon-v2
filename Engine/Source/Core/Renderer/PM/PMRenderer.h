#pragma once

#include "Core/Renderer/Renderer.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Filmic/SampleFilter.h"
#include "Core/Renderer/PM/EPMMode.h"
#include "Core/Renderer/PM/PMStatistics.h"

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
	void doUpdate(const CoreCookedUnit& cooked, const VisualWorld& world) override;
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

	void asyncMergeFilm(const HdrRgbFilm& srcFilm);
	void asyncReplaceFilm(const HdrRgbFilm& srcFilm);

private:
	std::unique_ptr<HdrRgbFilm> m_film;

	const Scene*          m_scene;
	const Receiver*       m_receiver;
	SampleGenerator*      m_sg;
	SampleFilter          m_filter;

	EPMMode m_mode;
	std::size_t m_numPhotons;
	std::size_t m_numPasses;
	std::size_t m_numSamplesPerPixel;
	real m_kernelRadius;

	std::mutex m_filmMutex;

	PMStatistics m_statistics;
	std::atomic_uint32_t m_photonsPerSecond;
	std::atomic_bool     m_isFilmUpdated;

	void renderWithVanillaPM();
	void renderWithProgressivePM();
	void renderWithStochasticProgressivePM();
};

}// end namespace ph
