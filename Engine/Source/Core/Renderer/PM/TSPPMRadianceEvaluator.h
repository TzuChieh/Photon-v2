#pragma once

#include "Core/Renderer/PM/TViewPathHandler.h"
#include "Core/Renderer/PM/TViewpoint.h"
#include "Core/Renderer/PM/TPhoton.h"
#include "Common/assertion.h"
#include "Common/primitive_type.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Emitter/Emitter.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Math/Random.h"
#include "World/Scene.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/LTABuildingBlock/PtDirectLightEstimator.h"
#include "Core/Renderer/PM/TPhotonMap.h"
#include "Math/math.h"

#include <vector>
#include <type_traits>
#include <utility>

namespace ph
{

template<typename Viewpoint, typename Photon>
class TSPPMRadianceEvaluator : public TViewPathHandler<TSPPMRadianceEvaluator<Viewpoint, Photon>>
{
	static_assert(std::is_base_of_v<TViewpoint<Viewpoint>, Viewpoint>);
	static_assert(std::is_base_of_v<TPhoton<Photon>, Photon>);

public:
	TSPPMRadianceEvaluator(
		Viewpoint* viewpoints,
		std::size_t numViewpoints,
		const TPhotonMap<Photon>* photonMap,
		std::size_t numPhotonPaths,
		const Scene* scene,
		HdrRgbFilm* film,
		std::size_t maxViewpointDepth);

	bool impl_onCameraSampleStart(
		const Vector2R&         filmNdc,
		const SpectralStrength& pathThroughput);

	auto impl_onPathHitSurface(
		std::size_t             pathLength,
		const SurfaceHit&       surfaceHit,
		const SpectralStrength& pathThroughput) -> ViewPathTracingPolicy;

	void impl_onCameraSampleEnd();

	void impl_onSampleBatchFinished();

private:
	Viewpoint* m_viewpoints;
	std::size_t m_numViewpoints;
	const TPhotonMap<Photon>* m_photonMap;
	std::size_t m_numPhotonPaths;
	const Scene* m_scene;
	HdrRgbFilm* m_film;
	std::size_t m_maxViewpointDepth;

	Viewpoint* m_viewpoint;
};

// In-header Implementations:

template<typename Viewpoint, typename Photon>
inline TSPPMRadianceEvaluator<Viewpoint, Photon>::TSPPMRadianceEvaluator(
	Viewpoint* viewpoints,
	std::size_t numViewpoints,
	const TPhotonMap<Photon>* photonMap,
	std::size_t numPhotonPaths,
	const Scene* scene,
	HdrRgbFilm* film,
	std::size_t maxViewpointDepth) :

	m_viewpoints(viewpoints),
	m_numViewpoints(numViewpoints),
	m_photonMap(photonMap),
	m_numPhotonPaths(numPhotonPaths),
	m_scene(scene),
	m_film(film),
	m_maxViewpointDepth(maxViewpointDepth)
{
	PH_ASSERT(m_viewpoints);
	PH_ASSERT(photonMap);
	PH_ASSERT_GE(numPhotonPaths, 1);
	PH_ASSERT(scene);
	PH_ASSERT(film);
	PH_ASSERT_GE(maxViewpointDepth, 1);
}

template<typename Viewpoint, typename Photon>
inline bool TSPPMRadianceEvaluator<Viewpoint, Photon>::impl_onCameraSampleStart(
	const Vector2R&         filmNdc,
	const SpectralStrength& pathThroughput)
{
	if(pathThroughput.isZero())
	{
		// TODO: should we add a 0-contribution viewpoint?
		return false;
	}

	const real fFilmXPx = filmNdc.x * static_cast<real>(m_film->getActualResPx().x);
	const real fFilmYPx = filmNdc.y * static_cast<real>(m_film->getActualResPx().y);
	const std::size_t filmX = std::min(static_cast<std::size_t>(fFilmXPx), static_cast<std::size_t>(m_film->getActualResPx().x) - 1);
	const std::size_t filmY = std::min(static_cast<std::size_t>(fFilmYPx), static_cast<std::size_t>(m_film->getActualResPx().y) - 1);

	const std::size_t viewpointIdx = filmY * static_cast<std::size_t>(m_film->getActualResPx().y) + filmX;
	PH_ASSERT_LT(viewpointIdx, m_numViewpoints);
	m_viewpoint = &(m_viewpoints[viewpointIdx]);

	return true;
}

template<typename Viewpoint, typename Photon>
inline auto TSPPMRadianceEvaluator<Viewpoint, Photon>::impl_onPathHitSurface(
	const std::size_t       pathLength,
	const SurfaceHit&       surfaceHit,
	const SpectralStrength& pathThroughput) -> ViewPathTracingPolicy
{
	// TODO
}

template<typename Viewpoint, typename Photon>
inline void TSPPMRadianceEvaluator<Viewpoint, Photon>::impl_onCameraSampleEnd()
{
	// TODO
}

template<typename Viewpoint, typename Photon>
inline void TSPPMRadianceEvaluator<Viewpoint, Photon>::impl_onSampleBatchFinished()
{}

}// end namespace ph