#pragma once

#include "Core/Renderer/PM/TPPMRadianceEvaluationWork.h"
#include "Core/Intersection/Primitive.h"
#include "Core/Intersection/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceHit.h"
#include "Core/Emitter/Emitter.h"
#include "Core/LTA/SurfaceTracer.h"
#include "Core/LTA/lta.h"
#include "Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/Renderer/PM/photon_map_light_transport.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Common/profiling.h>

#include <vector>

namespace ph
{

template<CPhoton Photon, CViewpoint Viewpoint>
inline TPPMRadianceEvaluationWork<Photon, Viewpoint>
::TPPMRadianceEvaluationWork(
	TSpan<Viewpoint>                     viewpoints,
	const TPhotonMap<Photon>* const      photonMap,
	const Scene* const                   scene,
	TSamplingFilm<math::Spectrum>* const film,
	const std::size_t                    totalPhotonPaths)

	: m_viewpoints               (viewpoints)
	, m_scene                    (scene)
	, m_photonMap                (photonMap)
	, m_film                     (film)
	, m_rcpTotalPhotonPaths      ()
	, m_statistics               (nullptr)
	, m_alpha                    ()
{
	PH_ASSERT(scene);
	PH_ASSERT(film);

	m_rcpTotalPhotonPaths = totalPhotonPaths > 0
		? 1.0_r / static_cast<real>(totalPhotonPaths)
		: 0.0_r;

	setStatistics(nullptr);
	setAlpha(2.0_r / 3.0_r);
}

template<CPhoton Photon, CViewpoint Viewpoint>
inline void TPPMRadianceEvaluationWork<Photon, Viewpoint>
::doWork()
{
	PH_PROFILE_SCOPE();

	sanitizeVariables();

	constexpr auto transport = lta::ETransport::Importance;
	constexpr auto sidednessPolicy = lta::ESidednessPolicy::Strict;

	const BsdfQueryContext bsdfContext(ALL_SURFACE_ELEMENTALS, transport, sidednessPolicy);
	const lta::SurfaceTracer surfaceTracer{m_scene};

	// For each viewpoint, evaluate radiance using collected data. If the viewpoint is for
	// view radiance only (and is on a delta optics), the evaluation would still work fine
	// (just the radius cannot shrink).

	std::vector<FullPhoton> photonCache;
	for(Viewpoint& viewpoint : m_viewpoints)
	{
		const SurfaceHit&    surfaceHit = viewpoint.template get<EViewpointData::SurfaceHit>();
		const math::Vector3R L          = viewpoint.template get<EViewpointData::ViewDir>();
		const math::Vector3R Ng         = surfaceHit.getGeometryNormal();
		const math::Vector3R Ns         = surfaceHit.getShadingNormal();
		const real           R          = viewpoint.template get<EViewpointData::Radius>();

		photonCache.clear();
		m_photonMap->find(surfaceHit.getPos(), R, photonCache);

		const real N    = viewpoint.template get<EViewpointData::NumPhotons>();
		const real M    = static_cast<real>(photonCache.size());
		const real newN = N + m_alpha * M;
		const real newR = (N + M) != 0.0_r ? R * std::sqrt(newN / (N + M)) : R;

		math::Spectrum tauM(0);
		BsdfEvalQuery  bsdfEval(bsdfContext);
		for(const auto& photon : photonCache)
		{
			const math::Vector3R V = photon.get<EPhotonData::FromDir>();
			if(!accept_photon_by_surface_topology(photon, Ng, Ns, L, V, bsdfContext.sidedness))
			{
				continue;
			}

			bsdfEval.inputs.set(surfaceHit, L, V);
			if(!surfaceTracer.doBsdfEvaluation(bsdfEval))
			{
				continue;
			}

			math::Spectrum tau = photon.get<EPhotonData::ThroughputRadiance>();
			tau.mulLocal(bsdfEval.outputs.getBsdf());
			tau.mulLocal(lta::tamed_importance_BSDF_Ns_corrector(Ns, Ng, V));

			tauM.addLocal(tau);
		}
		const math::Spectrum tauN   = viewpoint.template get<EViewpointData::Tau>();
		const math::Spectrum newTau = (N + M) != 0.0_r ? (tauN + tauM) * (newN / (N + M)) : math::Spectrum(0);

		viewpoint.template set<EViewpointData::Radius>(newR);
		viewpoint.template set<EViewpointData::NumPhotons>(newN);
		viewpoint.template set<EViewpointData::Tau>(newTau);
		
		// Evaluate radiance using current iteration's data

		const real kernelArea         = newR * newR * math::constant::pi<real>;
		const real radianceMultiplier = m_rcpTotalPhotonPaths / kernelArea;

		math::Spectrum radiance(viewpoint.template get<EViewpointData::Tau>() * radianceMultiplier);
		radiance.mulLocal(viewpoint.template get<EViewpointData::ViewThroughput>());
		radiance.addLocal(viewpoint.template get<EViewpointData::ViewRadiance>());

		const math::Vector2D rasterCoord = viewpoint.template get<EViewpointData::RasterCoord>();
		m_film->addSample(rasterCoord.x(), rasterCoord.y(), radiance);
	}
}

template<CPhoton Photon, CViewpoint Viewpoint>
inline void TPPMRadianceEvaluationWork<Photon, Viewpoint>
::sanitizeVariables()
{
	real sanitizedAlpha = m_alpha;
	if(m_alpha < 0.0_r || m_alpha > 1.0_r)
	{
		PH_DEFAULT_LOG(Warning,
			"alpha must be in [0, 1], {} provided, clamping", m_alpha);
		sanitizedAlpha = math::clamp(m_alpha, 0.0_r, 1.0_r);
	}

	m_alpha = sanitizedAlpha;
}

}// end namespace ph
