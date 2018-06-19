#pragma once

#include "Core/Renderer/Renderer.h"
#include "Core/Filmic/filmic_fwd.h"
#include "Core/Filmic/SampleFilter.h"

#include <vector>
#include <memory>

namespace ph
{

class Scene;
class Camera;
class SampleGenerator;
class Integrator;

class SamplingRenderer final : public Renderer, public TCommandInterface<SamplingRenderer>
{
public:
	virtual ~SamplingRenderer() override;

	AttributeTags supportedAttributes() const override;
	void init(const Description& description) override;
	bool asyncGetNewWork(uint32 workerId, RenderWork* out_work) override;
	void asyncSubmitWork(uint32 workerId, const RenderWork& work, bool isUpdating) override;
	ERegionStatus asyncPollUpdatedRegion(Region* out_region) override;

	void asyncDevelopFilmRegion(HdrRgbFrame& out_frame, const Region& region, EAttribute attribute) override;
	void develop(HdrRgbFrame& out_frame, EAttribute attribute) override;

private:
	std::unique_ptr<TSamplingFilm<SpectralStrength>> m_lightEnergyFilm;

	const Scene*          m_scene;
	SampleGenerator*      m_sg;
	Integrator*           m_integrator;
	Camera*               m_camera;
	SampleFilter          m_filter;

	uint32                                              m_numRemainingWorks;
	uint32                                              m_numFinishedWorks;
	std::vector<std::unique_ptr<SampleGenerator>>       m_workSgs;
	std::vector<std::unique_ptr<SpectralSamplingFilm>>  m_workFilms;
	std::deque<std::pair<Region, bool>>                 m_updatedRegions;
	
	std::mutex m_rendererMutex;

	void clearWorkData();
	void addUpdatedRegion(const Region& region, bool isUpdating);

// command interface
public:
	explicit SamplingRenderer(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph