#include "Core/Renderer/TiledRenderer.h"
#include "FileIO/Description.h"
#include "Core/Filmic/Film.h"

#include <utility>

namespace ph
{

TiledRenderer::TiledRenderer(const uint32 tileWpx, const uint32 tileHpx) : 
	m_tileWpx(tileWpx), m_tileHpx(tileHpx)
{

}

TiledRenderer::~TiledRenderer() = default;

void TiledRenderer::init(const Description& description)
{
	m_scene      = &description.visualWorld.getScene();
	m_sg         = description.getSampleGenerator().get();
	m_integrator = description.getIntegrator().get();
	m_film       = description.getFilm().get();
	m_camera     = description.getCamera().get();

	m_workSgs.resize(m_numThreads);
	m_workFilms.resize(m_numThreads);

	m_nextTilePosPx = m_film->getEffectiveWindowPx().calcCenter();
}

bool TiledRenderer::getNewWork(const uint32 workerId, RenderWork* out_work)
{
	std::lock_guard<std::mutex> lock(m_rendererMutex);

	TAABB2D<int64> workFilmWindow(m_nextTilePosPx, m_nextTilePosPx.add(m_tileWpx, m_tileHpx));
	workFilmWindow.intersectWith(m_film->getEffectiveWindowPx());
	if(!workFilmWindow.isValid())
	{
		return false;
	}

	// TODO

	return false;
}

void TiledRenderer::submitWork(const uint32 workerId, const RenderWork& work, bool isUpdating)
{
	std::lock_guard<std::mutex> lock(m_rendererMutex);

	// TODO
}

ERegionStatus TiledRenderer::asyncPollUpdatedRegion(Region* out_region)
{
	std::lock_guard<std::mutex> lock(m_rendererMutex);

	// TODO

	return ERegionStatus::INVALID;
}

void TiledRenderer::asyncDevelopFilmRegion(TFrame<real>& out_frame, const Region& region)
{
	std::lock_guard<std::mutex> lock(m_rendererMutex);

	m_film->develop(out_frame, region);
}

}// end namespace ph