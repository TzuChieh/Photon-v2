#pragma once

#include "Core/Renderer/Renderer.h"
#include "Core/Filmic/filmic_fwd.h"

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
	SamplingRenderer();
	virtual ~SamplingRenderer() override;

	AttributeTags supportedAttributes() const override;
	void init(const Description& description) override;
	bool getNewWork(uint32 workerId, RenderWork* out_work) override;
	void submitWork(uint32 workerId, const RenderWork& work, bool isUpdating) override;
	ERegionStatus asyncPollUpdatedRegion(Region* out_region) override;
	void asyncDevelopFilmRegion(HdrRgbFrame& out_frame, const Region& region) override;

private:
	SampleFilter m_filter;
	std::mutex m_filmMutex;

	std::unique_ptr<TSamplingFilm<SpectralStrength>> m_lightEnergy;

	const Scene*          m_scene;
	SampleGenerator*      m_sg;
	Integrator*           m_integrator;
	SpectralSamplingFilm* m_film;
	Camera*               m_camera;

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