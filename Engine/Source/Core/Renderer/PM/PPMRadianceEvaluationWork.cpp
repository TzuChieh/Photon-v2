#include "Core/Renderer/PM/PPMRadianceEvaluationWork.h"
#include "Common/assertion.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceHit.h"
#include "Core/Renderer/PM/PMRenderer.h"
#include "Core/Emitter/Emitter.h"
#include "Core/LTABuildingBlock/TSurfaceEventDispatcher.h"
#include "Core/LTABuildingBlock/lta.h"
#include "Common/Logger.h"

namespace ph
{

namespace
{
	const Logger logger(LogSender("PPM Radiance Evaluator"));
}

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
	TSurfaceEventDispatcher<ESaPolicy::STRICT> surfaceEvent(m_scene);

	std::vector<FullPhoton> photonCache;
	for(std::size_t i = 0; i < m_numViewpoints; ++i)
	{
		FullViewpoint& viewpoint = m_viewpoints[i];

		const SurfaceHit& surfaceHit = viewpoint.get<EViewpointData::SURFACE_HIT>();
		const Vector3R    L          = viewpoint.get<EViewpointData::VIEW_DIR>();
		const Vector3R    Ng         = surfaceHit.getGeometryNormal();
		const Vector3R    Ns         = surfaceHit.getShadingNormal();
		const real        R          = viewpoint.get<EViewpointData::RADIUS>();

		photonCache.clear();
		getPhotonMap()->findWithinRange(surfaceHit.getPosition(), R, photonCache);

		const real N    = viewpoint.get<EViewpointData::NUM_PHOTONS>();
		const real M    = static_cast<real>(photonCache.size());
		const real newN = N + m_alpha * M;
		const real newR = (N + M) != 0.0_r ? R * std::sqrt(newN / (N + M)) : R;

		SpectralStrength tauM(0);
		BsdfEvaluation   bsdfEval;
		for(const auto& photon : photonCache)
		{
			const Vector3R V = photon.get<EPhotonData::FROM_DIR>();

			bsdfEval.inputs.set(surfaceHit, L, V, ALL_ELEMENTALS, ETransport::IMPORTANCE);
			if(!surfaceEvent.doBsdfEvaluation(surfaceHit, bsdfEval))
			{
				continue;
			}

			SpectralStrength tau = photon.get<EPhotonData::THROUGHPUT_RADIANCE>();
			tau.mulLocal(bsdfEval.outputs.bsdf);
			tau.mulLocal(lta::importance_BSDF_Ns_corrector(Ns, Ng, L, V));

			tauM.addLocal(tau);
		}
		const SpectralStrength tauN   = viewpoint.get<EViewpointData::TAU>();
		const SpectralStrength newTau = (N + M) != 0.0_r ? (tauN + tauM) * (newN / (N + M)) : SpectralStrength(0);

		viewpoint.set<EViewpointData::RADIUS>(newR);
		viewpoint.set<EViewpointData::NUM_PHOTONS>(newN);
		viewpoint.set<EViewpointData::TAU>(newTau);
		
		// evaluate radiance using current iteration's data

		const real kernelArea         = newR * newR * PH_PI_REAL;
		const real radianceMultiplier = 1.0_r / (kernelArea * static_cast<real>(numPhotonPaths()));

		SpectralStrength radiance(viewpoint.get<EViewpointData::TAU>());
		radiance.mulLocal(radianceMultiplier);
		radiance.mulLocal(viewpoint.get<EViewpointData::VIEW_THROUGHPUT>());
		radiance.addLocal(viewpoint.get<EViewpointData::VIEW_RADIANCE>());

		const Vector2R filmNdc = viewpoint.get<EViewpointData::FILM_NDC>();
		const real filmXPx = filmNdc.x * static_cast<real>(m_film->getActualResPx().x);
		const real filmYPx = filmNdc.y * static_cast<real>(m_film->getActualResPx().y);
		m_film->addSample(filmXPx, filmYPx, radiance);
	}
}

void PPMRadianceEvaluationWork::sanitizeVariables()
{
	real sanitizedAlpha = m_alpha;
	if(m_alpha < 0.0_r || m_alpha > 1.0_r)
	{
		logger.log(ELogLevel::WARNING_MED, 
			"alpha must be in [0, 1], " + std::to_string(m_alpha) + " provided, clamping");

		sanitizedAlpha = math::clamp(m_alpha, 0.0_r, 1.0_r);
	}

	m_alpha = sanitizedAlpha;
}

}// end namespace ph
