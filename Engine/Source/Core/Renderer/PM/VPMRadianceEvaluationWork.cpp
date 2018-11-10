#include "Core/Renderer/PM/VPMRadianceEvaluationWork.h"
#include "Common/assertion.h"
#include "World/Scene.h"
#include "Core/Camera/Camera.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceHit.h"
#include "Core/Renderer/PM/PMRenderer.h"

namespace ph
{

VPMRadianceEvaluationWork::VPMRadianceEvaluationWork(
	const TPhotonMap<VPMPhoton>* photonMap,
	const std::size_t            numPhotonPaths,
	const Scene* const           scene,
	const Camera* const          camera,
	SampleGenerator* const       sampleGenerator,
	HdrRgbFilm* const            film) :

	TRadianceEvaluationWork(photonMap, numPhotonPaths),

	m_scene          (scene),
	m_camera         (camera),
	m_sampleGenerator(sampleGenerator),
	m_film           (film)
{
	PH_ASSERT(scene);
	PH_ASSERT(camera);
	PH_ASSERT(sampleGenerator);
	PH_ASSERT(film);

	setPMStatistics(nullptr);
	setPMRenderer(nullptr);
	setKernelRadius(0.1_r);
}

void VPMRadianceEvaluationWork::doWork()
{
	const real kernelArea         = m_kernelRadius * m_kernelRadius * PH_PI_REAL;
	const real radianceMultiplier = 1.0_r / (kernelArea * static_cast<real>(numPhotonPaths()));

	const std::size_t numPixels = static_cast<std::size_t>(m_film->getActualResPx().product());
	const Samples2DStage filmStage = m_sampleGenerator->declare2DStage(numPixels);// FIXME: consider sample filter extent & size hints

	std::vector<VPMPhoton> photonCache;

	while(m_sampleGenerator->prepareSampleBatch())
	{
		const Samples2D samples = m_sampleGenerator->getSamples2D(filmStage);
		for(std::size_t i = 0; i < samples.numSamples(); ++i)
		{
			const Vector2R filmNdcPos = samples[i];

			Ray tracingRay;
			m_camera->genSensedRay(filmNdcPos, &tracingRay);
			tracingRay.reverse();

			HitProbe probe;
			if(!m_scene->isIntersecting(tracingRay, &probe))
			{
				continue;
			}

			SurfaceHit surfaceHit(tracingRay, probe);
			const PrimitiveMetadata* metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
			const SurfaceOptics* surfaceOptics = metadata->getSurface().getOptics();

			// TODO: handle specular path

			const Vector3R V  = tracingRay.getDirection().mul(-1);
			const Vector3R Ng = surfaceHit.getGeometryNormal();
			const Vector3R Ns = surfaceHit.getShadingNormal();

			photonCache.clear();
			getPhotonMap()->findWithinRange(surfaceHit.getPosition(), m_kernelRadius, photonCache);

			BsdfEvaluation   bsdfEval;
			SpectralStrength radiance(0);
			for(const auto& photon : photonCache)
			{
				const Vector3R L = photon.get<EPhotonData::INCIDENT_DIR>();

				bsdfEval.inputs.set(surfaceHit, L, V, ALL_ELEMENTALS, ETransport::RADIANCE);
				surfaceOptics->calcBsdf(bsdfEval);

				if(!bsdfEval.outputs.isGood())
				{
					continue;
				}

				SpectralStrength throughput(1.0_r);// TODO: this is not true after ray bounces
				throughput.mulLocal(bsdfEval.outputs.bsdf);
				//throughput.mulLocal(Ns.absDot(V) * Ng.absDot(L) / Ng.absDot(V) / Ns.absDot(L));
				throughput.mulLocal(photon.get<EPhotonData::THROUGHPUT>());

				radiance.addLocal(throughput * photon.get<EPhotonData::RADIANCE>());
			}
			radiance.mulLocal(radianceMultiplier);

			const real filmXPx = filmNdcPos.x * static_cast<real>(m_film->getActualResPx().x);
			const real filmYPx = filmNdcPos.y * static_cast<real>(m_film->getActualResPx().y);
			m_film->addSample(filmXPx, filmYPx, radiance);
		}

		if(m_statistics)
		{
			m_statistics->asyncIncrementNumPasses();
		}

		if(m_renderer)
		{
			m_renderer->asyncMergeFilm(*m_film);
		}
	}// end while
}

}// end namespace ph
