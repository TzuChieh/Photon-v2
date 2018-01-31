#pragma once

#include "Core/Renderer/Renderer.h"

#include <vector>
#include <memory>

namespace ph
{

class Scene;
class Camera;
class SampleGenerator;
class Integrator;
class Film;

class BulkRenderer final : public Renderer
{
public:
	BulkRenderer();
	virtual ~BulkRenderer() override;

	virtual void init(const Description& description) override;
	virtual bool getNewWork(uint32 workerId, RenderWork* out_work) override;
	virtual void submitWork(uint32 workerId, const RenderWork& work, bool isUpdating) override;
	virtual ERegionStatus asyncPollUpdatedRegion(Region* out_region) override;
	virtual void asyncDevelopFilmRegion(HdrRgbFrame& out_frame, const Region& region) override;

private:
	const Scene*     m_scene;
	SampleGenerator* m_sg;
	Integrator*      m_integrator;
	Film*            m_film;
	Camera*          m_camera;

	uint32                                        m_numRemainingWorks;
	uint32                                        m_numFinishedWorks;
	std::vector<std::unique_ptr<SampleGenerator>> m_workSgs;
	std::vector<std::unique_ptr<Film>>            m_workFilms;
	std::deque<std::pair<Region, bool>>           m_updatedRegions;
	
	std::mutex m_rendererMutex;

	void clearWorkData();
	void addUpdatedRegion(const Region& region, bool isUpdating);
};

}// end namespace ph