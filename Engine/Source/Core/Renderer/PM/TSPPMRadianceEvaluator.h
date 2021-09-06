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
#include "Core/Renderer/PM/TPhotonMap.h"
#include "Math/math.h"
#include "Core/Scheduler/Region.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"

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
		const Region& filmRegion,
		std::size_t numSamplesPerPixel,
		std::size_t maxViewpointDepth);

	bool impl_onReceiverSampleStart(
		const math::Vector2D& rasterCoord,
		const Spectrum&       pathThroughput);

	auto impl_onPathHitSurface(
		std::size_t       pathLength,
		const SurfaceHit& surfaceHit,
		const Spectrum&   pathThroughput) -> ViewPathTracingPolicy;

	void impl_onReceiverSampleEnd();

	void impl_onSampleBatchFinished();

private:
	Viewpoint* m_viewpoints;
	std::size_t m_numViewpoints;
	const TPhotonMap<Photon>* m_photonMap;
	std::size_t m_numPhotonPaths;
	const Scene* m_scene;
	HdrRgbFilm* m_film;
	Region m_filmRegion;
	std::size_t m_numSamplesPerPixel;
	std::size_t m_maxViewpointDepth;

	Viewpoint* m_viewpoint;
	std::vector<Photon> m_photonCache;
	bool m_isViewpointFound;

	void addViewRadiance(const Spectrum& radiance);
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
	const Region& filmRegion,
	std::size_t numSamplesPerPixel,
	std::size_t maxViewpointDepth) :

	m_viewpoints(viewpoints),
	m_numViewpoints(numViewpoints),
	m_photonMap(photonMap),
	m_numPhotonPaths(numPhotonPaths),
	m_scene(scene),
	m_film(film),
	m_filmRegion(filmRegion),
	m_numSamplesPerPixel(numSamplesPerPixel),
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
inline bool TSPPMRadianceEvaluator<Viewpoint, Photon>::impl_onReceiverSampleStart(
	const math::Vector2D& rasterCoord,
	const Spectrum&       pathThroughput)
{
	// FIXME: sample res

	const auto fRasterCoord = math::Vector2R(rasterCoord);

	const math::Vector2S regionPosPx(math::Vector2R(
		math::clamp(
			fRasterCoord.x() - static_cast<real>(m_film->getEffectiveWindowPx().getMinVertex().x()),
			0.0_r,
			static_cast<real>(m_film->getEffectiveResPx().x() - 1)),
		math::clamp(
			fRasterCoord.y() - static_cast<real>(m_film->getEffectiveWindowPx().getMinVertex().y()),
			0.0_r, 
			static_cast<real>(m_film->getEffectiveResPx().y() - 1))));

	const std::size_t viewpointIdx = 
		regionPosPx.y() * static_cast<std::size_t>(m_film->getEffectiveResPx().x()) +
		regionPosPx.x();

	PH_ASSERT_LT(viewpointIdx, m_numViewpoints);
	m_viewpoint = &(m_viewpoints[viewpointIdx]);
	
	m_isViewpointFound = false;

	return true;
}

template<typename Viewpoint, typename Photon>
inline auto TSPPMRadianceEvaluator<Viewpoint, Photon>::impl_onPathHitSurface(
	const std::size_t pathLength,
	const SurfaceHit& surfaceHit,
	const Spectrum&   pathThroughput) -> ViewPathTracingPolicy
{
	const PrimitiveMetadata* metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
	const SurfaceOptics* optics = metadata->getSurface().getOptics();

	// TODO: MIS
	if constexpr(Viewpoint::template has<EViewpointData::VIEW_RADIANCE>())
	{
		if(metadata->getSurface().getEmitter())
		{
			Spectrum viewRadiance;
			metadata->getSurface().getEmitter()->evalEmittedRadiance(surfaceHit, &viewRadiance);
			addViewRadiance(pathThroughput * viewRadiance);
		}
	}
	
	// TODO: better handling of glossy optics
	if(optics->getAllPhenomena().hasAtLeastOne({ESurfacePhenomenon::DIFFUSE_REFLECTION}) ||
		pathLength >= m_maxViewpointDepth)
	{
		if constexpr(Viewpoint::template has<EViewpointData::SURFACE_HIT>()) {
			m_viewpoint->template set<EViewpointData::SURFACE_HIT>(surfaceHit);
		}
		if constexpr(Viewpoint::template has<EViewpointData::VIEW_THROUGHPUT>()) {
			m_viewpoint->template set<EViewpointData::VIEW_THROUGHPUT>(pathThroughput);
		}
		if constexpr(Viewpoint::template has<EViewpointData::VIEW_DIR>()) {
			m_viewpoint->template set<EViewpointData::VIEW_DIR>(surfaceHit.getIncidentRay().getDirection().mul(-1));
		}

		m_isViewpointFound = true;

		return ViewPathTracingPolicy().kill();
	}
	else
	{
		return ViewPathTracingPolicy().
			traceSinglePathFor(ALL_ELEMENTALS).
			useRussianRoulette(true);
	}
}

template<typename Viewpoint, typename Photon>
inline void TSPPMRadianceEvaluator<Viewpoint, Photon>::impl_onReceiverSampleEnd()
{
	if(!m_isViewpointFound)
	{
		return;
	}

	const SurfaceTracer surfaceTracer(m_scene);

	const SurfaceHit&    surfaceHit = m_viewpoint->template get<EViewpointData::SURFACE_HIT>();
	const math::Vector3R L          = m_viewpoint->template get<EViewpointData::VIEW_DIR>();
	const math::Vector3R Ng         = surfaceHit.getGeometryNormal();
	const math::Vector3R Ns         = surfaceHit.getShadingNormal();
	const real           R          = m_viewpoint->template get<EViewpointData::RADIUS>();

	m_photonCache.clear();
	m_photonMap->findWithinRange(surfaceHit.getPosition(), R, m_photonCache);

	// FIXME: as a parameter
	const real alpha = 2.0_r / 3.0_r;

	const real N    = m_viewpoint->template get<EViewpointData::NUM_PHOTONS>();
	const real M    = static_cast<real>(m_photonCache.size());
	const real newN = N + alpha * M;
	const real newR = (N + M) != 0.0_r ? R * std::sqrt(newN / (N + M)) : R;

	const BsdfQueryContext bsdfContext(ALL_ELEMENTALS, ETransport::IMPORTANCE, ESidednessPolicy::STRICT);

	Spectrum      tauM(0);
	BsdfEvalQuery bsdfEval(bsdfContext);
	for(const auto& photon : m_photonCache)
	{
		const math::Vector3R V = photon.template get<EPhotonData::FROM_DIR>();

		bsdfEval.inputs.set(surfaceHit, L, V);
		if(!surfaceTracer.doBsdfEvaluation(bsdfEval))
		{
			continue;
		}

		Spectrum tau = photon.template get<EPhotonData::THROUGHPUT_RADIANCE>();
		tau.mulLocal(bsdfEval.outputs.bsdf);
		tau.mulLocal(lta::importance_BSDF_Ns_corrector(Ns, Ng, L, V));

		tauM.addLocal(tau);
	}
	tauM.mulLocal(m_viewpoint->template get<EViewpointData::VIEW_THROUGHPUT>());

	const Spectrum tauN   = m_viewpoint->template get<EViewpointData::TAU>();
	const Spectrum newTau = (N + M) != 0.0_r ? (tauN + tauM) * (newN / (N + M)) : Spectrum(0);

	m_viewpoint->template set<EViewpointData::RADIUS>(newR);
	m_viewpoint->template set<EViewpointData::NUM_PHOTONS>(newN);
	m_viewpoint->template set<EViewpointData::TAU>(newTau);
}

template<typename Viewpoint, typename Photon>
inline void TSPPMRadianceEvaluator<Viewpoint, Photon>::impl_onSampleBatchFinished()
{
	// evaluate radiance using current iteration's data
	for(int64 y = m_filmRegion.getMinVertex().y(); y < m_filmRegion.getMaxVertex().y(); ++y)
	{
		for(int64 x = m_filmRegion.getMinVertex().x(); x < m_filmRegion.getMaxVertex().x(); ++x)
		{
			const std::size_t viewpointIdx =
				(y - m_film->getEffectiveWindowPx().getMinVertex().y()) * static_cast<std::size_t>(m_film->getEffectiveResPx().x()) +
				(x - m_film->getEffectiveWindowPx().getMinVertex().x());

			PH_ASSERT_LT(viewpointIdx, m_numViewpoints);
			const auto& viewpoint = m_viewpoints[viewpointIdx];

			const real radius             = viewpoint.template get<EViewpointData::RADIUS>();
			const real kernelArea         = radius * radius * math::constant::pi<real>;
			const real radianceMultiplier = 1.0_r / (kernelArea * static_cast<real>(m_numPhotonPaths));

			Spectrum radiance(viewpoint.template get<EViewpointData::TAU>() * radianceMultiplier);
			radiance.addLocal(viewpoint.template get<EViewpointData::VIEW_RADIANCE>() / static_cast<real>(m_numSamplesPerPixel));
			m_film->setPixel(static_cast<float64>(x), static_cast<float64>(y), radiance);
		}
	}
}

template<typename Viewpoint, typename Photon>
inline void TSPPMRadianceEvaluator<Viewpoint, Photon>::addViewRadiance(const Spectrum& radiance)
{
	if constexpr(Viewpoint::template has<EViewpointData::VIEW_RADIANCE>())
	{
		Spectrum viewRadiance = m_viewpoint->template get<EViewpointData::VIEW_RADIANCE>();
		viewRadiance.addLocal(radiance);
		m_viewpoint->template set<EViewpointData::VIEW_RADIANCE>(viewRadiance);
	}
}

}// end namespace ph
