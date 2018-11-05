#include "Core/Renderer/PPM/PhotonMappingWork.h"
#include "World/Scene.h"
#include "Core/Camera/Camera.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Ray.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceHit.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Math/Random.h"
#include "Core/Estimator/BuildingBlock/RussianRoulette.h"
#include "Common/assertion.h"

namespace ph
{

PhotonMappingWork::PhotonMappingWork(
	const Scene* scene,
	const Camera* camera,
	std::unique_ptr<SampleGenerator> sampleGenerator,
	Photon* photonBuffer,
	std::size_t numPhotons,
	std::size_t* out_numEmittedPhotons) :
	m_scene(scene),
	m_camera(camera),
	m_sampleGenerator(std::move(sampleGenerator)),
	m_photonBuffer(photonBuffer),
	m_numPhotons(numPhotons),
	m_numEmittedPhotons(out_numEmittedPhotons)
{}

void PhotonMappingWork::doWork()
{
	*m_numEmittedPhotons = 0;
	//int bounces = 0;
	for(std::size_t i = 0; i < m_numPhotons;)
	{
		++(*m_numEmittedPhotons);

		Ray tracingRay;
		SpectralStrength emittedRadiance;
		Vector3R emitN;
		real pdfA;
		real pdfW;
		m_scene->genSensingRay(&tracingRay, &emittedRadiance, &emitN, &pdfA, &pdfW);

		PH_ASSERT_MSG(emittedRadiance.isNonNegative(), "emittedRadiance = " + emittedRadiance.toString());

		SpectralStrength throughput(1.0_r);
		throughput.divLocal(pdfA);
		throughput.divLocal(pdfW);
		throughput.mulLocal(emitN.absDot(tracingRay.getDirection()));

		while(true)
		{
			HitProbe probe;
			if(!m_scene->isIntersecting(tracingRay, &probe))
			{
				// DEBUG
				//Photon photon;
				//photon.position = tracingRay.getOrigin();
				//photon.radiance.setValues(0);
				//photon.throughput.setValues(throughput);
				//photon.V = tracingRay.getDirection().mul(-1);
				//m_photonBuffer[i++] = photon;
				//PH_ASSERT_MSG(throughput.isNonNegative(), "throughput = " + throughput.toString());
				//if(i == m_numPhotons /*|| ++bounces == 5*/)
				//{
				//	break;
				//}

				break;
			}

			SurfaceHit surfaceHit(tracingRay, probe);
			const PrimitiveMetadata* metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
			const SurfaceOptics* optics = metadata->getSurface().getOptics();

			SpectralStrength weightedThroughput;
			if(RussianRoulette::surviveOnLuminance(throughput, &weightedThroughput))
			{
				throughput = weightedThroughput;

				Photon photon;
				photon.position = surfaceHit.getPosition();
				photon.radiance = emittedRadiance;
				photon.throughput.setValues(throughput);
				photon.V = tracingRay.getDirection().mul(-1);
				m_photonBuffer[i++] = photon;

				PH_ASSERT_MSG(throughput.isNonNegative(), "throughput = " + throughput.toString());

				if(i == m_numPhotons /*|| ++bounces == 5*/)
				{
					break;
				}
			}
			else
			{
				break;
			}

			BsdfSample bsdfSample;
			bsdfSample.inputs.set(surfaceHit, tracingRay.getDirection().mul(-1), ALL_ELEMENTALS, ETransport::IMPORTANCE);
			optics->calcBsdfSample(bsdfSample);
			if(!bsdfSample.outputs.isGood())
			{
				break;
			}

			Vector3R V = tracingRay.getDirection().mulLocal(-1);
			Vector3R L = bsdfSample.outputs.L;
			Vector3R Ng = surfaceHit.getGeometryNormal();
			Vector3R Ns = surfaceHit.getShadingNormal();
			throughput.mulLocal(bsdfSample.outputs.pdfAppliedBsdf);
			throughput.mulLocal(Ns.absDot(L));
			//throughput.mulLocal(Ns.absDot(V));
			//throughput.mulLocal(Ns.absDot(V) * Ng.absDot(L) / Ng.absDot(V) / Ns.absDot(L));

			tracingRay.setOrigin(surfaceHit.getPosition());
			tracingRay.setDirection(L);
		}
	}
}

}// end namespace ph