#pragma once

#include "Core/Renderer/Renderer.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Filmic/SampleFilter.h"
#include "Core/Renderer/Sampling/SamplingRenderWork.h"
#include "Core/Renderer/Sampling/SamplingFilmSet.h"

#include <vector>
#include <memory>
#include <atomic>

namespace ph
{

class Scene;
class Camera;
class SampleGenerator;
class Estimator;

class SamplingRenderer : public Renderer, public TCommandInterface<SamplingRenderer>
{
public:
	~SamplingRenderer() override;

	void doUpdate(const SdlResourcePack& data) override;
	void doRender() override;
	void develop(HdrRgbFrame& out_frame, EAttribute attribute) override;

	ERegionStatus asyncPollUpdatedRegion(Region* out_region) override;
	RenderState asyncQueryRenderState() override;
	RenderProgress asyncQueryRenderProgress() override;
	void asyncDevelopRegion(HdrRgbFrame& out_frame, const Region& region, EAttribute attribute) override;

	AttributeTags supportedAttributes() const override;
	std::string renderStateName(RenderState::EType type, std::size_t index) const override;

	void asyncUpdateFilm(SamplingRenderWork& work);

private:
	SamplingFilmSet m_films;

	const Scene*          m_scene;
	Camera*               m_camera;
	SampleGenerator*      m_sg;
	std::unique_ptr<Estimator> m_estimator;
	SampleFilter          m_filter;
	std::vector<SamplingRenderWork> m_works;

	// TODO: use ERegionStatus instead of bool
	std::deque<std::pair<Region, bool>>                 m_updatedRegions;
	
	std::mutex m_rendererMutex;
	std::atomic_uint m_percentageProgress;
	std::atomic_uint32_t m_samplesPerPixel;

	AttributeTags m_requestedAttributes;

	void clearWorkData();
	void mergeWorkFilms(SamplingRenderWork& work);
	void addUpdatedRegion(const Region& region, bool isUpdating);

// command interface
public:
	explicit SamplingRenderer(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph