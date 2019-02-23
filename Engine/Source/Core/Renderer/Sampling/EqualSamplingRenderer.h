#pragma once

#include "Core/Renderer/Sampling/SamplingRenderer.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Renderer/Sampling/CameraSamplingWork.h"
#include "Core/Renderer/Sampling/TCameraMeasurementEstimator.h"
#include "Core/Renderer/Region/WorkScheduler.h"
#include "Core/Renderer/Sampling/MetaRecordingProcessor.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Math/TVector2.h"

#include <vector>
#include <memory>
#include <atomic>
#include <functional>

namespace ph
{

class Scene;
class Camera;
class SampleGenerator;

class EqualSamplingRenderer : public SamplingRenderer, public TCommandInterface<EqualSamplingRenderer>
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
	using FilmEstimator = TCameraMeasurementEstimator<HdrRgbFilm, SpectralStrength>;

	enum class EScheduler
	{
		BULK,
		STRIPE,
		GRID,
		TILE,
		SPIRAL,
		SPIRAL_GRID
	};

	const Scene*                   m_scene;
	const Camera*                  m_camera;
	SampleGenerator*               m_sampleGenerator;
	HdrRgbFilm                     m_mainFilm;

	std::unique_ptr<WorkScheduler> m_scheduler;
	EScheduler                     m_schedulerType;
	Vector2S                       m_blockSize;
	
	std::vector<CameraSamplingWork>         m_renderWorks;
	std::vector<FilmEstimator>              m_filmEstimators;
	std::vector<MetaRecordingProcessor>     m_metaRecorders;

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
	void initScheduler(std::size_t numSamplesPerPixel);

// command interface
public:
	explicit EqualSamplingRenderer(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  renderer                   </category>
	<type_name> equal                      </type_name>
	<extend>    renderer.sampling-renderer </extend>

	<name> Equal Sampling Renderer </name>
	<description>
		This renderer renders images by path sampling techniques and 
		distributes them equally. Typically, this means the rendering 
		technique used is unbiased, and the the image converges as a
		whole.
	</description>

	<command type="creator">
		<input name="scheduler" type="string">
			<description>
				Scheduler for rendering, affect the order of rendered regions.
				Possible values: bulk, stripe, grid, tile, spiral, spiral-grid.
			</description>
		</input>
		<input name="block-width" type="integer">
			<description>Desired width for render scheduling.</description>
		</input>
		<input name="block-height" type="integer">
			<description>Desired height for render scheduling.</description>
		</input>
	</command>

	</SDL_interface>
*/