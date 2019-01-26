#pragma once

#include "Core/Renderer/Renderer.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Filmic/SampleFilter.h"
#include "Core/Renderer/Sampling/SamplingRenderWork.h"
#include "Core/Renderer/Sampling/SamplingFilmSet.h"
#include "Core/Renderer/Region/WorkScheduler.h"
#include "Core/Estimator/Estimator.h"

#include <vector>
#include <memory>
#include <atomic>
#include <functional>

namespace ph
{

class Scene;
class Camera;
class SampleGenerator;

class SamplingRenderer : public Renderer, public TCommandInterface<SamplingRenderer>
{
public:
	// Methods regarding supplying and submitting works are guaranteed to be
	// thread-safe. In addition, memory written by previous supply/submit
	// action is visible to the subsequent one.
	virtual bool supplyWork(uint32 workerId, SamplingRenderWork& work) = 0;
	virtual void submitWork(uint32 workerId, SamplingRenderWork& work) = 0;

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
	void asyncUpdateFilm(SamplingFilmSet& workerFilms, bool isUpdating);
	void asyncDevelop(HdrRgbFrame& out_frame, EAttribute attribute);
	AttributeTags getRequestedAttributes() const;
	std::size_t numAvailableSampleBatches() const;
	SampleFilter getFilter() const;
	SampleGenerator* getSampleGenerator() const;
	SamplingFilmSet* getSamplingFilms();

private:
	const Scene*               m_scene;
	const Camera*              m_camera;
	SampleGenerator*           m_sampleGenerator;
	std::unique_ptr<Estimator> m_estimator;
	SampleFilter               m_filter;
	SamplingFilmSet            m_films;
	AttributeTags              m_requestedAttributes;
	std::vector<SamplingRenderWork> m_renderWorks;

	struct UpdatedRegion
	{
		Region region;
		bool   isFinished;
	};
	std::deque<UpdatedRegion> m_updatedRegions;
	
	std::mutex           m_rendererMutex;
	std::atomic_uint32_t m_averageSpp;

	void clearWorkData();
	void mergeWorkFilms(SamplingFilmSet& films);
	void addUpdatedRegion(const Region& region, bool isUpdating);

// command interface
public:
	explicit SamplingRenderer(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

// In-header Implementations:

inline AttributeTags SamplingRenderer::getRequestedAttributes() const
{
	return m_requestedAttributes;
}

inline SampleFilter SamplingRenderer::getFilter() const
{
	return m_filter;
}

inline SampleGenerator* SamplingRenderer::getSampleGenerator() const
{
	return m_sampleGenerator;
}

inline SamplingFilmSet* SamplingRenderer::getSamplingFilms()
{
	return &m_films;
}

}// end namespace ph

/*
	<SDL_interface>

	<category>  renderer          </category>
	<type_name> sampling          </type_name>
	<extend>    renderer.renderer </extend>

	<name> Sampling Renderer </name>
	<description>
		This renderer renders images by path sampling techniques. Typically, this means the 
		rendering technique used is unbiased.
	</description>

	<command type="creator">
		<input name="filter-name" type="string">
			<description>
				The type of filter used by the film. "box": box filter, fairly sharp but can have
				obvious aliasing around edges; "gaussian": Gaussian filter, gives smooth results;
				"mitchell-netravali" or "mn": Mitchell-Netravali filter, smooth but remains sharp
				around edges; "blackman-harris" or "bh": Blackman-Harris filter, a good compromise
				between smoothness and sharpness.
			</description>
		</input>
		<input name="estimator" type="string">
			<description>
				The energy estimating component used by the renderer. "bvpt": backward path 
				tracing; "bneept": backward path tracing with next event estimation.
			</description>
		</input>
		<input name="light-energy-tag" type="string">
			<description>Renders light energy or not. Can be "true" or "false".</description>
		</input>
		<input name="normal-tag" type="string">
			<description>Renders normal vector or not. Can be "true" or "false".</description>
		</input>
	</command>

	</SDL_interface>
*/