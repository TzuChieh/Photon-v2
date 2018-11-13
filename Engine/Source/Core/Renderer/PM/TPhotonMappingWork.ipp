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
#include "Core/LTABuildingBlock/RussianRoulette.h"
#include "Common/assertion.h"
#include "Utility/Timer.h"
#include "Core/Renderer/PM/PMStatistics.h"
#include "Core/LTABuildingBlock/TSurfaceEventDispatcher.h"
#include "Core/LTABuildingBlock/lta.h"

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
	// FIXME: currently we exit immediately when photon buffer is full; we should trace a full path instead

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
		if(pdfA * pdfW == 0.0_r)
		{
			continue;
		}

		// here 0-bounce lighting is not accounted for

		SpectralStrength throughputRadiance(emittedRadiance);
		throughputRadiance.divLocal(pdfA);
		throughputRadiance.divLocal(pdfW);
		throughputRadiance.mulLocal(emitN.absDot(tracingRay.getDirection()));

		TSurfaceEventDispatcher<ESaPolicy::STRICT> surfaceEvent(m_scene);

		// start tracing single photon path
		while(!throughputRadiance.isZero())
		{
			SurfaceHit surfaceHit;
			if(!surfaceEvent.traceNextSurface(tracingRay, &surfaceHit))
			{
				break;
			}

			const PrimitiveMetadata* metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
			const SurfaceOptics* optics = metadata->getSurface().getOptics();

			SpectralStrength weightedThroughputRadiance;
			if(RussianRoulette::surviveOnLuminance(throughputRadiance, &weightedThroughputRadiance))
			{
				throughputRadiance = weightedThroughputRadiance;

				Photon photon;

				if constexpr(Photon::template has<EPhotonData::POSITION>()) {
					photon.template set<EPhotonData::POSITION>(surfaceHit.getPosition());
				}
				if constexpr(Photon::template has<EPhotonData::THROUGHPUT_RADIANCE>()) {
					photon.template set<EPhotonData::THROUGHPUT_RADIANCE>(throughputRadiance);
				}
				if constexpr(Photon::template has<EPhotonData::FROM_DIR>()) {
					photon.template set<EPhotonData::FROM_DIR>(tracingRay.getDirection().mul(-1));
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
			Ray sampledRay;
			bsdfSample.inputs.set(surfaceHit, tracingRay.getDirection().mul(-1), ALL_ELEMENTALS, ETransport::IMPORTANCE);
			if(!surfaceEvent.doBsdfSample(surfaceHit, bsdfSample, &sampledRay))
			{
				break;
			}

			Vector3R V = tracingRay.getDirection().mulLocal(-1);
			Vector3R L = bsdfSample.outputs.L;
			Vector3R Ng = surfaceHit.getGeometryNormal();
			Vector3R Ns = surfaceHit.getShadingNormal();
			throughputRadiance.mulLocal(bsdfSample.outputs.pdfAppliedBsdf);
			throughputRadiance.mulLocal(lta::importance_BSDF_Ns_corrector(Ns, Ng, L, V));
			throughputRadiance.mulLocal(Ns.absDot(L));

			tracingRay = sampledRay;
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