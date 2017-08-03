#pragma once

#include "Core/Renderer/Renderer.h"

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
	virtual ERegionStatus asyncPollUpdatedRegion(Region* out_region) override;
	virtual bool getNewWork(RenderWork* out_work) override;
	virtual void submitWork(const RenderWork& work, bool isUpdating) override;

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