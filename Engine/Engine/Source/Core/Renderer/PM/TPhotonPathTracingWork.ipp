#pragma once

#include "Core/Renderer/PM/TPhotonPathTracingWork.h"
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
#include "Core/LTA/RussianRoulette.h"
#include "Utility/Timer.h"
#include "Core/Renderer/PM/PMAtomicStatistics.h"
#include "Core/LTA/SurfaceTracer.h"
#include "Core/LTA/lta.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"

#include <Common/assertion.h>
#include <Common/profiling.h>

namespace ph
{

template<CPhoton Photon>
inline TPhotonPathTracingWork<Photon>::TPhotonPathTracingWork(

	const Scene* const     scene,
	const Receiver* const  receiver,
	SampleGenerator* const sampleGenerator,
	const TSpan<Photon>    photonBuffer,
	const uint32           minPhotonBounces,
	const uint32           maxPhotonBounces)

	: m_scene           (scene)
	, m_receiver        (receiver)
	, m_sampleGenerator (sampleGenerator)
	, m_photonBuffer    (photonBuffer)
	, m_minPhotonBounces(minPhotonBounces)
	, m_maxPhotonBounces(maxPhotonBounces)
	, m_numPhotonPaths  (0)
	, m_statistics      (nullptr)
{
	PH_ASSERT_GE(minPhotonBounces, 1);
	PH_ASSERT_LE(minPhotonBounces, maxPhotonBounces);
}

template<CPhoton Photon>
inline void TPhotonPathTracingWork<Photon>::doWork()
{
	PH_PROFILE_SCOPE();

	// FIXME: currently we exit immediately when photon buffer is full; we should trace a full path instead

	Timer timer;
	timer.start();

	const BsdfQueryContext bsdfContext(ALL_ELEMENTALS, ETransport::Importance, lta::ESidednessPolicy::Strict);
	const lta::SurfaceTracer surfaceTracer{m_scene};

	const auto raySampleHandle = m_sampleGenerator->declareStageND(2, m_photonBuffer.size());
	m_sampleGenerator->prepareSampleBatch();// HACK: check if succeeded
	auto raySamples = m_sampleGenerator->getSamplesND(raySampleHandle);

	m_numPhotonPaths               = 0;
	std::size_t numStoredPhotons   = 0;
	std::size_t photonStatsCounter = 0;
	while(numStoredPhotons < m_photonBuffer.size())
	{
		++m_numPhotonPaths;

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

		// Here 0-bounce lighting is never accounted for
		math::Spectrum throughputRadiance(emittedRadiance);
		throughputRadiance.divLocal(pdfA);
		throughputRadiance.divLocal(pdfW);
		throughputRadiance.mulLocal(emitN.absDot(tracingRay.getDirection()));

		// Start tracing single photon path with at least 1 bounce
		uint32 numPhotonBounces = 0;
		while(!throughputRadiance.isZero())
		{
			PH_ASSERT_LT(numPhotonBounces, m_maxPhotonBounces);

			SurfaceHit surfaceHit;
			if(!surfaceTracer.traceNextSurface(tracingRay, bsdfContext.sidedness, &surfaceHit))
			{
				break;
			}

			++numPhotonBounces;

			const PrimitiveMetadata* metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
			const SurfaceOptics* optics = metadata->getSurface().getOptics();

			math::Spectrum weightedThroughputRadiance;
			if(lta::RussianRoulette{}.surviveOnLuminance(throughputRadiance, sampleFlow, &weightedThroughputRadiance))
			{
				throughputRadiance = weightedThroughputRadiance;

				if(numPhotonBounces >= m_minPhotonBounces)
				{
					m_photonBuffer[numStoredPhotons++] = makePhoton(
						surfaceHit, throughputRadiance, tracingRay);
					++photonStatsCounter;
				}

				if(numStoredPhotons == m_photonBuffer.size() || 
				   numPhotonBounces == m_maxPhotonBounces)
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

		if(photonStatsCounter >= 16384)
		{
			timer.stop();
			setElapsedMs(timer.getDeltaMs());

			if(m_statistics)
			{
				m_statistics->addNumTracedPhotons(photonStatsCounter);
			}

			photonStatsCounter = 0;
		}
	}// end while photon buffer is not full

	timer.stop();
	setElapsedMs(timer.getDeltaMs());

	if(m_statistics)
	{
		m_statistics->addNumTracedPhotons(photonStatsCounter);
	}
}

template<CPhoton Photon>
inline void TPhotonPathTracingWork<Photon>::setStatistics(PMAtomicStatistics* const statistics)
{
	m_statistics = statistics;
}

template<CPhoton Photon>
inline std::size_t TPhotonPathTracingWork<Photon>::numPhotonPaths() const
{
	return m_numPhotonPaths;
}

template<CPhoton Photon>
inline Photon TPhotonPathTracingWork<Photon>::makePhoton(
	const SurfaceHit&     surfaceHit, 
	const math::Spectrum& throughputRadiance,
	const Ray&            tracingRay)
{
	Photon photon;
	if constexpr(Photon::template has<EPhotonData::Position>())
	{
		photon.template set<EPhotonData::Position>(surfaceHit.getPosition());
	}
	if constexpr(Photon::template has<EPhotonData::ThroughputRadiance>())
	{
		photon.template set<EPhotonData::ThroughputRadiance>(throughputRadiance);
	}
	if constexpr(Photon::template has<EPhotonData::FromDir>())
	{
		photon.template set<EPhotonData::FromDir>(tracingRay.getDirection().mul(-1));
	}

	return photon;
}

}// end namespace ph
