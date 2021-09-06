#include "Core/Renderer/PM/PPMRadianceEvaluationWork.h"
#include "Common/assertion.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceHit.h"
#include "Core/Renderer/PM/PMRenderer.h"
#include "Core/Emitter/Emitter.h"
#include "Core/LTABuildingBlock/SurfaceTracer.h"
#include "Core/LTABuildingBlock/lta.h"
#include "Common/logging.h"
#include "Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"

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

	setPMStatistics(nullptr);
	setAlpha(2.0_r / 3.0_r);
}

void PPMRadianceEvaluationWork::doWork()
{
	sanitizeVariables();

	const BsdfQueryContext bsdfContext(ALL_ELEMENTALS, ETransport::IMPORTANCE, ESidednessPolicy::STRICT);
	const SurfaceTracer    surfaceTracer(m_scene);

	std::vector<FullPhoton> photonCache;
	for(std::size_t i = 0; i < m_numViewpoints; ++i)
	{
		FullViewpoint& viewpoint = m_viewpoints[i];

		const SurfaceHit&    surfaceHit = viewpoint.get<EViewpointData::SURFACE_HIT>();
		const math::Vector3R L          = viewpoint.get<EViewpointData::VIEW_DIR>();
		const math::Vector3R Ng         = surfaceHit.getGeometryNormal();
		const math::Vector3R Ns         = surfaceHit.getShadingNormal();
		const real           R          = viewpoint.get<EViewpointData::RADIUS>();

		photonCache.clear();
		getPhotonMap()->findWithinRange(surfaceHit.getPosition(), R, photonCache);

		const real N    = viewpoint.get<EViewpointData::NUM_PHOTONS>();
		const real M    = static_cast<real>(photonCache.size());
		const real newN = N + m_alpha * M;
		const real newR = (N + M) != 0.0_r ? R * std::sqrt(newN / (N + M)) : R;

		Spectrum      tauM(0);
		BsdfEvalQuery bsdfEval(bsdfContext);
		for(const auto& photon : photonCache)
		{
			const math::Vector3R V = photon.get<EPhotonData::FROM_DIR>();

			bsdfEval.inputs.set(surfaceHit, L, V);
			if(!surfaceTracer.doBsdfEvaluation(bsdfEval))
			{
				continue;
			}

			Spectrum tau = photon.get<EPhotonData::THROUGHPUT_RADIANCE>();
			tau.mulLocal(bsdfEval.outputs.bsdf);
			tau.mulLocal(lta::importance_BSDF_Ns_corrector(Ns, Ng, L, V));

			tauM.addLocal(tau);
		}
		const Spectrum tauN   = viewpoint.get<EViewpointData::TAU>();
		const Spectrum newTau = (N + M) != 0.0_r ? (tauN + tauM) * (newN / (N + M)) : Spectrum(0);

		viewpoint.set<EViewpointData::RADIUS>(newR);
		viewpoint.set<EViewpointData::NUM_PHOTONS>(newN);
		viewpoint.set<EViewpointData::TAU>(newTau);
		
		// evaluate radiance using current iteration's data

		const real kernelArea         = newR * newR * math::constant::pi<real>;
		const real radianceMultiplier = 1.0_r / (kernelArea * static_cast<real>(numPhotonPaths()));

		Spectrum radiance(viewpoint.get<EViewpointData::TAU>() * radianceMultiplier);
		radiance.addLocal(viewpoint.get<EViewpointData::VIEW_RADIANCE>());
		radiance.mulLocal(viewpoint.get<EViewpointData::VIEW_THROUGHPUT>());

		const math::Vector2D rasterCoord = viewpoint.get<EViewpointData::RASTER_COORD>();
		m_film->addSample(rasterCoord.x(), rasterCoord.y(), radiance);
	}
}

void PPMRadianceEvaluationWork::sanitizeVariables()
{
	real sanitizedAlpha = m_alpha;
	if(m_alpha < 0.0_r || m_alpha > 1.0_r)
	{
		PH_LOG_WARNING(PPMRadianceEvaluationWork, "alpha must be in [0, 1], {} provided, clamping", m_alpha);

		sanitizedAlpha = math::clamp(m_alpha, 0.0_r, 1.0_r);
	}

	m_alpha = sanitizedAlpha;
}

}// end namespace ph
