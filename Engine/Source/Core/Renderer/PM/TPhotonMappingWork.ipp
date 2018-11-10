#pragma once

#include "Core/Renderer/PM/TPhotonMappingWork.h"
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
#include "Utility/Timer.h"
#include "Core/Renderer/PM/PMStatistics.h"

namespace ph
{

template<typename Photon>
inline TPhotonMappingWork<Photon>::TPhotonMappingWork(

	const Scene* const     scene,
	const Camera* const    camera,
	SampleGenerator* const sampleGenerator,
	Photon* const          photonBuffer,
	const std::size_t      numPhotons,
	std::size_t* const     out_numPhotonPaths) :

	m_scene(scene),
	m_camera(camera),
	m_sampleGenerator(sampleGenerator),
	m_photonBuffer(photonBuffer),
	m_numPhotons(numPhotons),
	m_numPhotonPaths(out_numPhotonPaths)
{
	setPMStatistics(nullptr);
}

template<typename Photon>
inline void TPhotonMappingWork<Photon>::doWork()
{
	Timer timer;
	timer.start();

	std::size_t numStoredPhotons = 0;
	*m_numPhotonPaths            = 0;
	std::size_t photonCounter    = 0;
	while(numStoredPhotons < m_numPhotons)
	{
		++(*m_numPhotonPaths);

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

		// start tracing single photon path
		while(true)
		{
			HitProbe probe;
			if(!m_scene->isIntersecting(tracingRay, &probe))
			{
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

				if constexpr(Photon::template has<EPhotonData::POSITION>())
				{
					photon.template set<EPhotonData::POSITION>(surfaceHit.getPosition());
				}

				if constexpr(Photon::template has<EPhotonData::RADIANCE>())
				{
					photon.template set<EPhotonData::RADIANCE>(emittedRadiance);
				}

				if constexpr(Photon::template has<EPhotonData::THROUGHPUT>())
				{
					photon.template set<EPhotonData::THROUGHPUT>(throughput);
				}

				if constexpr(Photon::template has<EPhotonData::INCIDENT_DIR>())
				{
					photon.template set<EPhotonData::INCIDENT_DIR>(tracingRay.getDirection().mul(-1));
				}

				m_photonBuffer[numStoredPhotons++] = photon;
				++photonCounter;

				if(numStoredPhotons == m_numPhotons)
				{
					break;
				}
			}// end if photon survived
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
			//throughput.mulLocal(Ns.absDot(V) * Ng.absDot(L) / Ng.absDot(V) / Ns.absDot(L));

			tracingRay.setOrigin(surfaceHit.getPosition());
			tracingRay.setDirection(L);
		}// end single photon path

		if(photonCounter >= 16384)
		{
			timer.finish();
			setElapsedMs(timer.getDeltaMs());

			if(m_statistics)
			{
				m_statistics->asyncAddNumTracedPhotons(photonCounter);
			}

			photonCounter = 0;
		}
	}// end while photon buffer is not full

	timer.finish();
	setElapsedMs(timer.getDeltaMs());

	if(m_statistics)
	{
		m_statistics->asyncAddNumTracedPhotons(photonCounter);
	}
}

template<typename Photon>
inline void TPhotonMappingWork<Photon>::setPMStatistics(PMStatistics* const statistics)
{
	m_statistics = statistics;
}

}// end namespace ph