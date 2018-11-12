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

namespace ph
{

PPMRadianceEvaluationWork::PPMRadianceEvaluationWork(
	const TPhotonMap<PPMPhoton>* photonMap,
	const std::size_t            numPhotonPaths,
	HdrRgbFilm* const            film,
	PPMViewpoint* viewpoints,
	std::size_t numViewpoints,
	const Scene* scene) :

	TRadianceEvaluationWork(photonMap, numPhotonPaths),

	m_film           (film),
	m_viewpoints(viewpoints),
	m_numViewpoints(numViewpoints),
	m_scene(scene)
{
	PH_ASSERT(film);

	setPMStatistics(nullptr);
}

void PPMRadianceEvaluationWork::doWork()
{
	const real alpha = 0.5_r;
	
	TSurfaceEventDispatcher<ESaPolicy::STRICT> surfaceEvent(m_scene);

	std::vector<PPMPhoton> photonCache;
	for(std::size_t i = 0; i < m_numViewpoints; ++i)
	{
		PPMViewpoint& viewpoint = m_viewpoints[i];

		const SurfaceHit& surfaceHit = viewpoint.get<EViewpointData::SURFACE_HIT>();
		const PrimitiveMetadata* metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
		const SurfaceOptics* surfaceOptics = metadata->getSurface().getOptics();

		const Vector3R V = viewpoint.get<EViewpointData::VIEW_DIR>();
		const Vector3R Ng = surfaceHit.getGeometryNormal();
		const Vector3R Ns = surfaceHit.getShadingNormal();
		const real R = viewpoint.get<EViewpointData::RADIUS>();

		photonCache.clear();
		getPhotonMap()->findWithinRange(surfaceHit.getPosition(), R, photonCache);

		const real N = viewpoint.get<EViewpointData::NUM_PHOTONS>();
		const real M = static_cast<real>(photonCache.size());
		const real newN = N + alpha * M;
		const real newR = (N + M) != 0.0_r ? R * std::sqrt(newN / (N + M)) : R;

		BsdfEvaluation bsdfEval;
		

		SpectralStrength tauN = viewpoint.get<EViewpointData::TAU>();
		SpectralStrength tauM(0);
		for(const auto& photon : photonCache)
		{
			const Vector3R L = photon.get<EPhotonData::FROM_DIR>();

			bsdfEval.inputs.set(surfaceHit, L, V, ALL_ELEMENTALS, ETransport::RADIANCE);
			if(!surfaceEvent.doBsdfEvaluation(surfaceHit, bsdfEval))
			{
				continue;
			}

			SpectralStrength tau = photon.get<EPhotonData::THROUGHPUT_RADIANCE>();
			tau.mulLocal(bsdfEval.outputs.bsdf);
			tau.mulLocal(Ns.absDot(L) * Ng.absDot(V) / Ng.absDot(L) / Ns.absDot(V));
			//throughput.mulLocal(Ns.absDot(L) / Ng.absDot(L));
			//throughput.mulLocal(Ns.absDot(V) / Ng.absDot(V));
			//throughput.mulLocal(Ng.absDot(V) / Ns.absDot(V));


			tauM.addLocal(tau);
		}
		const SpectralStrength newTau = (N + M) != 0.0_r ? (tauN + tauM) * (newN / (N + M)) : SpectralStrength(0);

		viewpoint.set<EViewpointData::RADIUS>(newR);
		viewpoint.set<EViewpointData::NUM_PHOTONS>(newN);
		viewpoint.set<EViewpointData::TAU>(newTau);
		
		const real kernelArea = newR * newR * PH_PI_REAL;
		const real radianceMultiplier = 1.0_r / (kernelArea * static_cast<real>(numPhotonPaths()));

		SpectralStrength radiance(viewpoint.get<EViewpointData::TAU>());
		radiance.mulLocal(viewpoint.get<EViewpointData::VIEW_THROUGHPUT>());
		radiance.mulLocal(radianceMultiplier);
		radiance.addLocal(viewpoint.get<EViewpointData::VIEW_RADIANCE>());

		const Vector2R filmNdc = viewpoint.get<EViewpointData::FILM_NDC>();
		const real filmXPx = filmNdc.x * static_cast<real>(m_film->getActualResPx().x);
		const real filmYPx = filmNdc.y * static_cast<real>(m_film->getActualResPx().y);
		m_film->addSample(filmXPx, filmYPx, radiance);
	}
}

}// end namespace ph
