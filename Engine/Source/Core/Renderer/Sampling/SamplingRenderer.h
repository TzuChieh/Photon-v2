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

class SamplingRenderer final : public Renderer, public TCommandInterface<SamplingRenderer>
{
public:
	~SamplingRenderer() override;

	AttributeTags supportedAttributes() const override;
	void init(const SdlResourcePack& data) override;
	bool asyncSupplyWork(RenderWorker& worker) override;
	void asyncSubmitWork(RenderWorker& worker) override;
	ERegionStatus asyncPollUpdatedRegion(Region* out_region) override;
	RenderStates asyncQueryRenderStates() override;

	void asyncUpdateFilm(SamplingRenderWork& work);

	void asyncDevelopFilmRegion(HdrRgbFrame& out_frame, const Region& region, EAttribute attribute) override;
	void develop(HdrRgbFrame& out_frame, EAttribute attribute) override;

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