#pragma once

#include "Core/Renderer/Renderer.h"
#include "Math/TVector2.h"

#include <vector>
#include <memory>
#include <mutex>

namespace ph
{

class Scene;
class Camera;
class SampleGenerator;
class Integrator;

class TiledRenderer final : public Renderer
{
public:
	TiledRenderer(uint32 tileWpx, uint32 tileHpx);
	virtual ~TiledRenderer() override;

	virtual void init(const Description& description) override;
	virtual bool getNewWork(uint32 workerId, RenderWork* out_work) override;
	virtual void submitWork(uint32 workerId, const RenderWork& work, bool isUpdating) override;
	virtual ERegionStatus asyncPollUpdatedRegion(Region* out_region) override;
	virtual void asyncDevelopFilmRegion(HdrRgbFrame& out_frame, const Region& region) override;

private:
	uint32 m_tileWpx, m_tileHpx;
	std::mutex m_rendererMutex;

	/*const Scene*     m_scene;
	SampleGenerator* m_sg;
	Integrator*      m_integrator;
	Film*            m_film;
	Camera*          m_camera;*/

	std::vector<std::unique_ptr<SampleGenerator>> m_workSgs;
	std::vector<std::unique_ptr<Film>>            m_workFilms;

	TVector2<int64> m_nextTilePosPx;
};

}// end namespace ph