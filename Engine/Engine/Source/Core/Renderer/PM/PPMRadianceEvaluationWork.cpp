#include "Core/Renderer/PM/PPMRadianceEvaluationWork.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceHit.h"
#include "Core/Emitter/Emitter.h"
#include "Core/LTA/SurfaceTracer.h"
#include "Core/LTA/lta.h"
#include "Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Common/profiling.h>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(PPMRadianceEvaluationWork, PhotonMap);

PPMRadianceEvaluationWork::PPMRadianceEvaluationWork(

	const TPhotonMap<FullPhoton>* const photonMap,
	const std::size_t                   numPhotonPaths,

	HdrRgbFilm* const    film,
	FullViewpoint* const viewpoints,
	const std::size_t    numViewpoints,
	const Scene* const   scene) :

	TRadianceEvaluationWork(photonMap, numPhotonPaths),

	m_film         (film),
	m_viewpoints   (viewpoints),
	m_numViewpoints(numViewpoints),
	m_scene        (scene)
{
	PH_ASSERT(film);

	setStatistics(nullptr);
	setAlpha(2.0_r / 3.0_r);
}

void PPMRadianceEvaluationWork::doWork()
{
	PH_PROFILE_SCOPE();

	sanitizeVariables();

	const BsdfQueryContext bsdfContext(ALL_ELEMENTALS, ETransport::Importance, lta::ESidednessPolicy::Strict);
	const lta::SurfaceTracer surfaceTracer{m_scene};

	std::vector<FullPhoton> photonCache;
	for(std::size_t i = 0; i < m_numViewpoints; ++i)
	{
		FullViewpoint& viewpoint = m_viewpoints[i];

		const SurfaceHit&    surfaceHit = viewpoint.get<EViewpointData::SurfaceHit>();
		const math::Vector3R L          = viewpoint.get<EViewpointData::ViewDir>();
		const math::Vector3R Ng         = surfaceHit.getGeometryNormal();
		const math::Vector3R Ns         = surfaceHit.getShadingNormal();
		const real           R          = viewpoint.get<EViewpointData::Radius>();

		photonCache.clear();
		getPhotonMap()->map.findWithinRange(surfaceHit.getPosition(), R, photonCache);

		const real N    = viewpoint.get<EViewpointData::NumPhotons>();
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
			tau.mulLocal(bsdfEval.outputs.bsdf);
			tau.mulLocal(lta::tamed_importance_BSDF_Ns_corrector(Ns, Ng, V));

			tauM.addLocal(tau);
		}
		const math::Spectrum tauN   = viewpoint.get<EViewpointData::Tau>();
		const math::Spectrum newTau = (N + M) != 0.0_r ? (tauN + tauM) * (newN / (N + M)) : math::Spectrum(0);

		viewpoint.set<EViewpointData::Radius>(newR);
		viewpoint.set<EViewpointData::NumPhotons>(newN);
		viewpoint.set<EViewpointData::Tau>(newTau);
		
		// Evaluate radiance using current iteration's data

		const real kernelArea         = newR * newR * math::constant::pi<real>;
		const real radianceMultiplier = 1.0_r / (kernelArea * static_cast<real>(numPhotonPaths()));

		math::Spectrum radiance(viewpoint.get<EViewpointData::Tau>() * radianceMultiplier);
		radiance.addLocal(viewpoint.get<EViewpointData::ViewRadiance>());
		radiance.mulLocal(viewpoint.get<EViewpointData::ViewThroughput>());

		const math::Vector2D rasterCoord = viewpoint.get<EViewpointData::RasterCoord>();
		m_film->addSample(rasterCoord.x(), rasterCoord.y(), radiance);
	}
}

void PPMRadianceEvaluationWork::sanitizeVariables()
{
	real sanitizedAlpha = m_alpha;
	if(m_alpha < 0.0_r || m_alpha > 1.0_r)
	{
		PH_LOG_WARNING(PPMRadianceEvaluationWork,
			"alpha must be in [0, 1], {} provided, clamping", m_alpha);
		sanitizedAlpha = math::clamp(m_alpha, 0.0_r, 1.0_r);
	}

	m_alpha = sanitizedAlpha;
}

}// end namespace ph
