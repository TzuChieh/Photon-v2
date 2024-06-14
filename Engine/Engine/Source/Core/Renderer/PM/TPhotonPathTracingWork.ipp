#pragma once

#include "Core/Renderer/PM/TPhotonPathTracingWork.h"
#include "World/Scene.h"
#include "Core/Receiver/Receiver.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Ray.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Intersection/Primitive.h"
#include "Core/Intersection/PrimitiveMetadata.h"
#include "Core/SurfaceHit.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Core/LTA/RussianRoulette.h"
#include "Utility/Timer.h"
#include "Core/Renderer/PM/PMAtomicStatistics.h"
#include "Core/LTA/SurfaceTracer.h"
#include "Core/LTA/lta.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/Emitter/Query/EnergyEmissionSampleQuery.h"

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
	const uint32           minPhotonPathLength,
	const uint32           maxPhotonPathLength)

	: m_scene              (scene)
	, m_receiver           (receiver)
	, m_sampleGenerator    (sampleGenerator)
	, m_photonBuffer       (photonBuffer)
	, m_minPhotonPathLength(minPhotonPathLength)
	, m_maxPhotonPathLength(maxPhotonPathLength)
	, m_numPhotonPaths     (0)
	, m_statistics         (nullptr)
{
	PH_ASSERT_GE(minPhotonPathLength, 1);
	PH_ASSERT_LE(minPhotonPathLength, maxPhotonPathLength);
}

template<CPhoton Photon>
inline void TPhotonPathTracingWork<Photon>::doWork()
{
	PH_PROFILE_SCOPE();

	// FIXME: currently we exit immediately when photon buffer is full; we should trace a full path instead

	Timer timer;
	timer.start();

	const BsdfQueryContext bsdfContext(ALL_SURFACE_ELEMENTALS, ETransport::Importance, lta::ESidednessPolicy::Strict);
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

		// TODO: properly sample time
		EnergyEmissionSampleQuery energyEmission;
		energyEmission.inputs.set(Time{});

		SurfaceHit surfaceHit;
		Ray tracingRay;
		{
			HitProbe probe;
			m_scene->emitRay(energyEmission, sampleFlow, probe);
			if(!energyEmission.outputs)
			{
				continue;
			}

			constexpr SurfaceHitReason reason(ESurfaceHitReason::SampledPosDir);
			tracingRay = energyEmission.outputs.getEmittedRay();
			surfaceHit = SurfaceHit(tracingRay, probe, reason);
		}

		PH_ASSERT_IN_RANGE(tracingRay.getDirection().lengthSquared(), 0.9_r, 1.1_r);

		// Here 0-bounce lighting is never accounted for
		math::Spectrum throughputRadiance(energyEmission.outputs.getEmittedEnergy());
		throughputRadiance.divLocal(energyEmission.outputs.getPdfA());
		throughputRadiance.divLocal(energyEmission.outputs.getPdfW());
		throughputRadiance.mulLocal(surfaceHit.getShadingNormal().absDot(tracingRay.getDirection()));

		// Start tracing single photon path with at least 1 bounce
		uint32 photonPathLength = 0;
		while(!throughputRadiance.isZero())
		{
			PH_ASSERT_LT(photonPathLength, m_maxPhotonPathLength);

			if(!surfaceTracer.traceNextSurfaceFrom(
				surfaceHit, tracingRay, bsdfContext.sidedness, &surfaceHit))
			{
				break;
			}

			++photonPathLength;

			// TODO: we can also skip storing this photon if the BSDF has little contribution
			// (e.g., by measuring its integrated value)

			const PrimitiveMetadata* metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
			const SurfaceOptics* optics = metadata->getSurface().getOptics();

			math::Spectrum weightedThroughputRadiance;
			if(lta::RussianRoulette{}.surviveOnLuminance(throughputRadiance, sampleFlow, &weightedThroughputRadiance))
			{
				throughputRadiance = weightedThroughputRadiance;

				if(photonPathLength >= m_minPhotonPathLength)
				{
					m_photonBuffer[numStoredPhotons++] = makePhoton(
						surfaceHit, throughputRadiance, tracingRay, photonPathLength);
					++photonStatsCounter;
				}

				if(numStoredPhotons == m_photonBuffer.size() || 
				   photonPathLength == m_maxPhotonPathLength)
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
			math::Vector3R L = bsdfSample.outputs.getL();
			math::Vector3R Ng = surfaceHit.getGeometryNormal();
			math::Vector3R Ns = surfaceHit.getShadingNormal();
			throughputRadiance.mulLocal(bsdfSample.outputs.getPdfAppliedBsdfCos());
			throughputRadiance.mulLocal(lta::tamed_importance_scatter_Ns_corrector(Ns, Ng, L, V));

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
	const Ray&            tracingRay,
	const std::size_t     pathLength)
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
	if constexpr(Photon::template has<EPhotonData::GeometryNormal>())
	{
		photon.template set<EPhotonData::GeometryNormal>(surfaceHit.getGeometryNormal());
	}
	if constexpr(Photon::template has<EPhotonData::PathLength>())
	{
		photon.template set<EPhotonData::PathLength>(pathLength);
	}

	return photon;
}

}// end namespace ph
