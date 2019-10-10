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

class PMRenderer : public Renderer, public TCommandInterface<PMRenderer>
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

	void asyncMergeFilm(const HdrRgbFilm& srcFilm);
	void asyncReplaceFilm(const HdrRgbFilm& srcFilm);

private:
	std::unique_ptr<HdrRgbFilm> m_film;

	const Scene*          m_scene;
	const Camera*         m_camera;
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

// command interface
public:
	explicit PMRenderer(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  renderer          </category>
	<type_name> pm                </type_name>
	<extend>    renderer.renderer </extend>

	<name> Photon Map Renderer </name>
	<description>
		This renderer renders images by utilizing a precomputed photon map. Like all caching based
		methods, this render technique is biased; rendered result converges to ground truth in 
		the limit.
	</description>

	<command type="creator">
		<input name="mode" type="string">
			<description>
				Photon mapping mode. "vanilla": directly compute energy values from photon map, no
				fancy tricks applied; "progressive": progressively refine the rendered results;
				"stochastic-progressive": stochastic sampling technique is utilized for energy 
				value computation.
			</description>
		</input>
		<input name="num-photons" type="integer">
			<description>
				Number of photons used. For progressive techniques, this value is for single pass.
			</description>
		</input>
		<input name="radius" type="real">
			<description>
				Contributing radius for each photon. For progressive techniques, this value is for
				setting up initial radius.
			</description>
		</input>
		<input name="num-passes" type="integer">
			<description>
				Number of passes performed by progressive techniques.
			</description>
		</input>
		<input name="num-samples-per-pixel" type="integer">
			<description>
				Number of samples per pixel. Higher values can resolve image aliasing, but can 
				consume large amounts of memory.
			</description>
		</input>
	</command>

	</SDL_interface>
*/
