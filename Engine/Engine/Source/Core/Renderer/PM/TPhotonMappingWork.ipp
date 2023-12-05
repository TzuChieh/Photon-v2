#pragma once

#include "Core/Renderer/PM/TPhotonMappingWork.h"
#include "World/Scene.h"
#include "Core/Receiver/Receiver.h"
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
#include "Utility/Timer.h"
#include "Core/Renderer/PM/PMStatistics.h"
#include "Core/LTABuildingBlock/SurfaceTracer.h"
#include "Core/LTABuildingBlock/lta.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"

#include <Common/assertion.h>

namespace ph
{

template<typename Photon>
inline TPhotonMappingWork<Photon>::TPhotonMappingWork(

	const Scene* const     scene,
	const Receiver* const  receiver,
	SampleGenerator* const sampleGenerator,
	Photon* const          photonBuffer,
	const std::size_t      numPhotons,
	std::size_t* const     out_numPhotonPaths) :

	m_scene(scene),
	m_receiver(receiver),
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

	const BsdfQueryContext bsdfContext(ALL_ELEMENTALS, ETransport::IMPORTANCE, ESidednessPolicy::STRICT);
	const SurfaceTracer surfaceTracer(m_scene);

	const auto raySampleHandle = m_sampleGenerator->declareStageND(2, m_numPhotons);
	m_sampleGenerator->prepareSampleBatch();// HACK: check if succeeded
	auto raySamples = m_sampleGenerator->getSamplesND(raySampleHandle);

	std::size_t numStoredPhotons = 0;
	*m_numPhotonPaths            = 0;
	std::size_t photonCounter    = 0;
	while(numStoredPhotons < m_numPhotons)
	{
		++(*m_numPhotonPaths);

		SampleFlow sampleFlow = raySamples.readSampleAsFlow();

		Ray tracingRay;
		math::Spectrum emittedRadiance;
		math::Vector3R emitN;
		real pdfA;
		real pdfW;
		m_scene->emitRay(sampleFlow, &tracingRay, &emittedRadiance, &emitN, &pdfA, &pdfW);
		if(pdfA * pdfW == 0.0_r)
		{
			continue;
		}

		// here 0-bounce lighting is not accounted for

		math::Spectrum throughputRadiance(emittedRadiance);
		throughputRadiance.divLocal(pdfA);
		throughputRadiance.divLocal(pdfW);
		throughputRadiance.mulLocal(emitN.absDot(tracingRay.getDirection()));

		// start tracing single photon path
		while(!throughputRadiance.isZero())
		{
			SurfaceHit surfaceHit;
			if(!surfaceTracer.traceNextSurface(tracingRay, bsdfContext.sidedness, &surfaceHit))
			{
				break;
			}

			const PrimitiveMetadata* metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
			const SurfaceOptics* optics = metadata->getSurface().getOptics();

			math::Spectrum weightedThroughputRadiance;
			if(RussianRoulette::surviveOnLuminance(throughputRadiance, sampleFlow, &weightedThroughputRadiance))
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

			BsdfSampleQuery bsdfSample(bsdfContext);
			Ray sampledRay;
			bsdfSample.inputs.set(surfaceHit, tracingRay.getDirection().mul(-1));
			if(!surfaceTracer.doBsdfSample(bsdfSample, sampleFlow, &sampledRay))
			{
				break;
			}

			math::Vector3R V = tracingRay.getDirection().mul(-1);
			math::Vector3R L = bsdfSample.outputs.L;
			math::Vector3R Ng = surfaceHit.getGeometryNormal();
			math::Vector3R Ns = surfaceHit.getShadingNormal();
			throughputRadiance.mulLocal(bsdfSample.outputs.pdfAppliedBsdf);
			throughputRadiance.mulLocal(lta::importance_BSDF_Ns_corrector(Ns, Ng, L, V));
			throughputRadiance.mulLocal(Ns.absDot(L));

			tracingRay = sampledRay;
		}// end single photon path

		if(photonCounter >= 16384)
		{
			timer.stop();
			setElapsedMs(timer.getDeltaMs());

			if(m_statistics)
			{
				m_statistics->asyncAddNumTracedPhotons(photonCounter);
			}

			photonCounter = 0;
		}
	}// end while photon buffer is not full

	timer.stop();
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
