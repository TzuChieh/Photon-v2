#pragma once

#include "Core/Renderer/PM/TViewPathHandler.h"
#include "Core/Renderer/PM/TViewpoint.h"
#include "Core/Renderer/PM/TPhoton.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Emitter/Emitter.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Math/math.h"
#include "Math/Random.h"
#include "Math/Color/Spectrum.h"
#include "World/Scene.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Core/Renderer/PM/TPhotonMap.h"
#include "Core/Scheduler/Region.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Utility/TSpan.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <vector>
#include <type_traits>
#include <utility>

namespace ph
{

template<CViewpoint Viewpoint, CPhoton Photon>
class TSPPMRadianceEvaluator : public TViewPathHandler<TSPPMRadianceEvaluator<Viewpoint, Photon>>
{
	static_assert(std::is_base_of_v<TViewpoint<Viewpoint>, Viewpoint>);
	static_assert(std::is_base_of_v<TPhoton<Photon>, Photon>);

public:
	TSPPMRadianceEvaluator(
		TSpan<Viewpoint> viewpoints,
		const TPhotonMap<Photon>* photonMap,
		std::size_t numPhotonPaths,
		const Scene* scene,
		TSamplingFilm<math::Spectrum>* film,
		const Region& statisticsRegion,
		const math::TVector2<int64>& statisticsRes,
		std::size_t numViewRadianceSamples,
		std::size_t maxViewpointDepth);

	bool impl_onReceiverSampleStart(
		const math::Vector2D& rasterCoord,
		const math::Vector2S& sampleIndex,
		const math::Spectrum& pathThroughput);

	auto impl_onPathHitSurface(
		std::size_t           pathLength,
		const SurfaceHit&     surfaceHit,
		const math::Spectrum& pathThroughput) -> ViewPathTracingPolicy;

	void impl_onReceiverSampleEnd();

	void impl_onSampleBatchFinished();

private:
	void addViewRadiance(Viewpoint& viewpoint, const math::Spectrum& radiance);
	math::Spectrum estimateRadiance(const Viewpoint& viewpoint) const;
	std::size_t getViewpointIndex(int64 sampleX, int64 sampleY) const;

	TSpan<Viewpoint> m_viewpoints;
	const TPhotonMap<Photon>* m_photonMap;
	real m_rcpNumPhotonPaths;
	const Scene* m_scene;
	TSamplingFilm<math::Spectrum>* m_film;
	Region m_statisticsRegion;
	math::TVector2<int64> m_statisticsRes;
	real m_rcpNumViewRadianceSamples;
	std::size_t m_maxViewpointDepth;

	Viewpoint* m_viewpoint;
	std::vector<Photon> m_photonCache;
	bool m_foundTargetHitPoint;
};

// In-header Implementations:

template<CViewpoint Viewpoint, CPhoton Photon>
inline TSPPMRadianceEvaluator<Viewpoint, Photon>::TSPPMRadianceEvaluator(
	const TSpan<Viewpoint> viewpoints,
	const TPhotonMap<Photon>* const photonMap,
	const std::size_t numPhotonPaths,
	const Scene* const scene,
	TSamplingFilm<math::Spectrum>* const film,
	const Region& statisticsRegion,
	const math::TVector2<int64>& statisticsRes,
	const std::size_t numViewRadianceSamples,
	const std::size_t maxViewpointDepth)

	: m_viewpoints(viewpoints)
	, m_photonMap(photonMap)
	, m_rcpNumPhotonPaths()
	, m_scene(scene)
	, m_film(film)
	, m_statisticsRegion(statisticsRegion)
	, m_statisticsRes(statisticsRes)
	, m_rcpNumViewRadianceSamples()
	, m_maxViewpointDepth(maxViewpointDepth)

	, m_viewpoint(nullptr)
	, m_photonCache()
	, m_foundTargetHitPoint(false)
{
	PH_ASSERT(!m_viewpoints.empty());
	PH_ASSERT(photonMap);
	PH_ASSERT_GE(numPhotonPaths, 1);
	PH_ASSERT(scene);
	PH_ASSERT(film);
	PH_ASSERT_GE(statisticsRes.product(), 1);
	PH_ASSERT_GE(maxViewpointDepth, 1);

	m_rcpNumPhotonPaths = numPhotonPaths > 0
		? 1.0_r / static_cast<real>(numPhotonPaths)
		: 0.0_r;

	m_rcpNumViewRadianceSamples = numViewRadianceSamples > 0
		? 1.0_r / static_cast<real>(numViewRadianceSamples)
		: 0.0_r;
}

template<CViewpoint Viewpoint, CPhoton Photon>
inline bool TSPPMRadianceEvaluator<Viewpoint, Photon>::impl_onReceiverSampleStart(
	const math::Vector2D& rasterCoord,
	const math::Vector2S& sampleIndex,
	const math::Spectrum& pathThroughput)
{
	m_viewpoint = &(m_viewpoints[getViewpointIndex(sampleIndex.x(), sampleIndex.y())]);
	m_foundTargetHitPoint = false;

	return true;
}

template<CViewpoint Viewpoint, CPhoton Photon>
inline auto TSPPMRadianceEvaluator<Viewpoint, Photon>::impl_onPathHitSurface(
	const std::size_t     pathLength,
	const SurfaceHit&     surfaceHit,
	const math::Spectrum& pathThroughput) -> ViewPathTracingPolicy
{
	const PrimitiveMetadata* metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
	const SurfaceOptics* optics = metadata->getSurface().getOptics();

	// TODO: MIS
	if constexpr(Viewpoint::template has<EViewpointData::ViewRadiance>())
	{
		if(metadata->getSurface().getEmitter())
		{
			math::Spectrum viewRadiance;
			metadata->getSurface().getEmitter()->evalEmittedRadiance(surfaceHit, &viewRadiance);
			addViewRadiance(*m_viewpoint, pathThroughput * viewRadiance);
		}
	}
	
	// TODO: better handling of glossy optics
	if(optics->getAllPhenomena().hasAny({ESurfacePhenomenon::DiffuseReflection}) ||
		pathLength >= m_maxViewpointDepth)
	{
		if constexpr(Viewpoint::template has<EViewpointData::SurfaceHit>())
		{
			m_viewpoint->template set<EViewpointData::SurfaceHit>(surfaceHit);
		}
		if constexpr(Viewpoint::template has<EViewpointData::ViewThroughput>())
		{
			m_viewpoint->template set<EViewpointData::ViewThroughput>(pathThroughput);
		}
		if constexpr(Viewpoint::template has<EViewpointData::ViewDir>())
		{
			m_viewpoint->template set<EViewpointData::ViewDir>(surfaceHit.getIncidentRay().getDirection().mul(-1));
		}

		m_foundTargetHitPoint = true;

		return ViewPathTracingPolicy().kill();
	}
	else
	{
		return ViewPathTracingPolicy().
			traceSinglePathFor(ALL_ELEMENTALS).
			useRussianRoulette(true);
	}
}

template<CViewpoint Viewpoint, CPhoton Photon>
inline void TSPPMRadianceEvaluator<Viewpoint, Photon>::impl_onReceiverSampleEnd()
{
	if(!m_foundTargetHitPoint)
	{
		return;
	}

	const SurfaceTracer surfaceTracer(m_scene);

	const SurfaceHit&    surfaceHit = m_viewpoint->template get<EViewpointData::SurfaceHit>();
	const math::Vector3R L          = m_viewpoint->template get<EViewpointData::ViewDir>();
	const math::Vector3R Ng         = surfaceHit.getGeometryNormal();
	const math::Vector3R Ns         = surfaceHit.getShadingNormal();
	const real           R          = m_viewpoint->template get<EViewpointData::Radius>();

	m_photonCache.clear();
	m_photonMap->map.findWithinRange(surfaceHit.getPosition(), R, m_photonCache);

	// FIXME: as a parameter
	const real alpha = 2.0_r / 3.0_r;

	const real N    = m_viewpoint->template get<EViewpointData::NumPhotons>();
	const real M    = static_cast<real>(m_photonCache.size());
	const real newN = N + alpha * M;
	const real newR = (N + M) != 0.0_r ? R * std::sqrt(newN / (N + M)) : R;

	const BsdfQueryContext bsdfContext(ALL_ELEMENTALS, ETransport::Importance, ESidednessPolicy::Strict);

	math::Spectrum tauM(0);
	BsdfEvalQuery  bsdfEval(bsdfContext);
	for(const auto& photon : m_photonCache)
	{
		const math::Vector3R V = photon.template get<EPhotonData::FromDir>();

		bsdfEval.inputs.set(surfaceHit, L, V);
		if(!surfaceTracer.doBsdfEvaluation(bsdfEval))
		{
			continue;
		}

		math::Spectrum tau = photon.template get<EPhotonData::ThroughputRadiance>();
		tau.mulLocal(bsdfEval.outputs.bsdf);
		tau.mulLocal(lta::importance_BSDF_Ns_corrector(Ns, Ng, L, V));

		tauM.addLocal(tau);
	}
	tauM.mulLocal(m_viewpoint->template get<EViewpointData::ViewThroughput>());

	const math::Spectrum tauN   = m_viewpoint->template get<EViewpointData::Tau>();
	const math::Spectrum newTau = (N + M) != 0.0_r ? (tauN + tauM) * (newN / (N + M)) : math::Spectrum(0);

	m_viewpoint->template set<EViewpointData::Radius>(newR);
	m_viewpoint->template set<EViewpointData::NumPhotons>(newN);
	m_viewpoint->template set<EViewpointData::Tau>(newTau);
}

template<CViewpoint Viewpoint, CPhoton Photon>
inline void TSPPMRadianceEvaluator<Viewpoint, Photon>::impl_onSampleBatchFinished()
{
	PH_ASSERT_GT(m_statisticsRes.product(), 0);

	const math::TAABB2D<float64> statisticsRegion(m_statisticsRegion);
	const auto rcpStatisticsRes = math::Vector2D(m_statisticsRes).rcp();

	// Evaluate radiance using current iteration's data. Each SPPM statistics gathered is for
	// estimating average radiance for a small sub-pixel region. We treat each estimated value
	// as if it is centered on the sub-pixel region it represents.
	for(int64 y = 0; y < m_statisticsRes.y(); ++y)
	{
		for(int64 x = 0; x < m_statisticsRes.x(); ++x)
		{
			const auto& viewpoint = m_viewpoints[getViewpointIndex(x, y)];
			const auto& statisticsRasterCoord = statisticsRegion.xy01ToSurface(
				(math::Vector2D(math::TVector2<int64>{x, y}) + 0.5) * rcpStatisticsRes);

			// For most accurate result, we should average the viewpoints' radiance by ourselves and
			// use `setPixel()` to store the value directly. Using `addSample()` will only be correct
			// if a box filter is used. Nevertheless, the worst result is a slightly overblurred image.
			// If higher number of viewpoints/regions per pixel were used, it will still approach a
			// per-sample filtered PT result in the limit (infinite viewpoints per pixel) except on
			// edges as we are not storing out-of-raster-bound statistics.
			m_film->addSample(
				statisticsRasterCoord.x(), 
				statisticsRasterCoord.y(), 
				estimateRadiance(viewpoint));
		}
	}
}

template<CViewpoint Viewpoint, CPhoton Photon>
inline void TSPPMRadianceEvaluator<Viewpoint, Photon>::addViewRadiance(
	Viewpoint& viewpoint, 
	const math::Spectrum& radiance)
{
	if constexpr(Viewpoint::template has<EViewpointData::ViewRadiance>())
	{
		math::Spectrum viewRadiance = m_viewpoint->template get<EViewpointData::ViewRadiance>();
		viewRadiance.addLocal(radiance);
		viewpoint.template set<EViewpointData::ViewRadiance>(viewRadiance);
	}
}

template<CViewpoint Viewpoint, CPhoton Photon>
inline math::Spectrum TSPPMRadianceEvaluator<Viewpoint, Photon>::estimateRadiance(
	const Viewpoint& viewpoint) const
{
	const real radius             = viewpoint.template get<EViewpointData::Radius>();
	const real kernelArea         = radius * radius * math::constant::pi<real>;
	const real radianceMultiplier = m_rcpNumPhotonPaths / kernelArea;
	const auto tau                = viewpoint.template get<EViewpointData::Tau>();
	const auto viewRadiance       = viewpoint.template get<EViewpointData::ViewRadiance>();

	math::Spectrum radiance(tau * radianceMultiplier);
	radiance.addLocal(viewRadiance * m_rcpNumViewRadianceSamples);
	return radiance;
}

template<CViewpoint Viewpoint, CPhoton Photon>
inline std::size_t TSPPMRadianceEvaluator<Viewpoint, Photon>::getViewpointIndex(
	const int64 sampleX, const int64 sampleY) const
{
	PH_ASSERT_IN_RANGE(sampleX, 0, m_statisticsRes.x());
	PH_ASSERT_IN_RANGE(sampleY, 0, m_statisticsRes.y());

	const std::size_t viewpointIdx = sampleY * m_statisticsRes.x() + sampleX;
	PH_ASSERT_LT(viewpointIdx, m_viewpoints.size());

	return viewpointIdx;
}

}// end namespace ph
