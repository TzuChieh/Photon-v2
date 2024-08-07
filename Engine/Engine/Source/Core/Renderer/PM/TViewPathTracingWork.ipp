#pragma once

#include "Core/Renderer/PM/TViewPathTracingWork.h"
#include "World/Scene.h"
#include "Core/Receiver/Receiver.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Ray.h"
#include "Core/HitDetail.h"
#include "Core/HitProbe.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersection/Primitive.h"
#include "Core/Intersection/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/LTA/SurfaceTracer.h"
#include "Core/LTA/RussianRoulette.h"
#include "Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Math/Geometry/TAABB2D.h"
#include "Math/math.h"
#include "Math/Random/sample.h"

#include <Common/assertion.h>
#include <Common/profiling.h>

#include <algorithm>

namespace ph
{

template<CViewPathHandler Handler>
inline TViewPathTracingWork<Handler>::TViewPathTracingWork(
	Handler* const                handler,
	const Scene* const            scene,
	const Receiver* const         receiver,
	SampleGenerator* const        sampleGenerator,
	const math::TAABB2D<float64>& rasterSampleWindowPx,
	const math::TVector2<int64>&  sampleRes)

	: m_handler             (handler)
	, m_scene               (scene)
	, m_receiver            (receiver)
	, m_sampleGenerator     (sampleGenerator)
	, m_rasterSampleWindowPx(rasterSampleWindowPx)
	, m_sampleRes           (sampleRes)
{
	PH_ASSERT_GT(sampleRes.product(), 0);
}

template<CViewPathHandler Handler>
inline const math::Vector2S& TViewPathTracingWork<Handler>::getSampleRes() const
{
	return m_sampleRes;
}

template<CViewPathHandler Handler>
inline void TViewPathTracingWork<Handler>::doWork()
{
	PH_PROFILE_SCOPE();
	PH_ASSERT(m_handler);
	PH_ASSERT_GT(m_sampleRes.product(), 0);

	const auto rasterSampleHandle = m_sampleGenerator->declareStageND(
		2,
		m_sampleRes.product(),
		{m_sampleRes.x(), m_sampleRes.y()});

	const auto raySampleHandle = m_sampleGenerator->declareStageND(2, m_sampleRes.product());

	while(m_sampleGenerator->prepareSampleBatch())
	{
		const auto rasterSamples = m_sampleGenerator->getSamplesND(rasterSampleHandle);
		auto raySamples = m_sampleGenerator->getSamplesND(raySampleHandle);
		for(std::size_t i = 0; i < rasterSamples.numSamples(); ++i)
		{
			// OPT: could be tiled; this has bad cache behavior (sample may jump around too far)

			const auto rasterSample = math::sample_cast<float64>(rasterSamples.template get<2>(i));
			const math::Vector2D rasterCoord = m_rasterSampleWindowPx.sampleToSurface(rasterSample);
			const math::Vector2S sampleIndex(math::Vector2D(
				math::clamp(m_sampleRes.x() * rasterSample[0], 0.0, m_sampleRes.x() - 1.0),
				math::clamp(m_sampleRes.y() * rasterSample[1], 0.0, m_sampleRes.y() - 1.0)));

			PH_ASSERT_LT(sampleIndex.x(), m_sampleRes.x());
			PH_ASSERT_LT(sampleIndex.y(), m_sampleRes.y());

			Ray tracingRay;
			const auto quantityWeight = m_receiver->receiveRay(rasterCoord, &tracingRay);
			tracingRay.reverse();

			math::Spectrum pathThroughput(quantityWeight);
			if(!m_handler->onReceiverSampleStart(rasterCoord, sampleIndex, pathThroughput))
			{
				m_handler->onReceiverSampleEnd();
				continue;
			}

			SampleFlow sampleFlow = raySamples.readSampleAsFlow();
			real rrScale = 1.0_r;

			traceViewPath(
				SurfaceHit{},
				tracingRay, 
				pathThroughput, 
				0,
				sampleFlow,
				rrScale);
			
			m_handler->onReceiverSampleEnd();
		}// end for single sample

		m_handler->onSampleBatchFinished();
	}// end while single sample batch
}

template<CViewPathHandler Handler>
inline void TViewPathTracingWork<Handler>::traceViewPath(
	SurfaceHit                   prevHit,
	Ray                          tracingRay,
	math::Spectrum               pathThroughput,
	std::size_t                  pathLength,
	SampleFlow&                  sampleFlow,
	real                         rrScale)
{	
	constexpr auto transport = lta::ETransport::Radiance;
	constexpr auto sidednessPolicy = lta::ESidednessPolicy::Strict;

	const lta::SurfaceTracer surfaceTracer{m_scene};
	const lta::RussianRoulette rr{};

	while(true)
	{
		SurfaceHit X;
		if(pathLength == 0)
		{
			if(!surfaceTracer.traceNextSurface(tracingRay, BsdfQueryContext{}.sidedness, &X))
			{
				break;
			}
		}
		else
		{
			if(!surfaceTracer.traceNextSurfaceFrom(
				prevHit, tracingRay, BsdfQueryContext{}.sidedness, &X))
			{
				break;
			}
		}

		++pathLength;
		const ViewPathTracingPolicy& policy = m_handler->onPathHitSurface(pathLength, X, pathThroughput);
		if(policy.isKilled())
		{
			break;
		}

		PH_ASSERT_IN_RANGE(tracingRay.getDir().lengthSquared(), 0.9_r, 1.1_r);

		const math::Vector3R V = -tracingRay.getDir();

		if(policy.getSampleMode() == EViewPathSampleMode::SinglePath)
		{
			BsdfSampleQuery bsdfSample(BsdfQueryContext(policy.getTargetElemental(), transport, sidednessPolicy));
			bsdfSample.inputs.set(X, V);

			Ray sampledRay;
			if(!surfaceTracer.doBsdfSample(bsdfSample, sampleFlow, &sampledRay))
			{
				break;
			}

			pathThroughput *= bsdfSample.outputs.getPdfAppliedBsdfCos();

			// Prevent premature termination of the path due to solid angle compression/expansion
			rrScale /= bsdfSample.outputs.getRelativeIor2();

			if(policy.useRussianRoulette())
			{
				real rrSurvivalProb;
				if(rr.surviveOnLuminance(pathThroughput * rrScale, sampleFlow, &rrSurvivalProb))
				{
					pathThroughput *= 1.0_r / rrSurvivalProb;
				}
				else
				{
					break;
				}
			}

			tracingRay = sampledRay;
		}
		else
		{
			traceElementallyBranchedPath(
				policy, 
				X, 
				V, 
				pathThroughput, 
				pathLength, 
				sampleFlow, 
				rrScale);
			break;
		}

		prevHit = X;
	}// end while true
}

template<CViewPathHandler Handler>
inline void TViewPathTracingWork<Handler>::traceElementallyBranchedPath(
	const ViewPathTracingPolicy& policy,
	const SurfaceHit&            X,
	const math::Vector3R&        V,
	const math::Spectrum&        pathThroughput,
	const std::size_t            pathLength,
	SampleFlow&                  sampleFlow,
	real                         rrScale)
{
	PH_ASSERT(policy.getSampleMode() == EViewPathSampleMode::ElementalBranch);

	constexpr auto transport = lta::ETransport::Radiance;
	constexpr auto sidednessPolicy = lta::ESidednessPolicy::Strict;

	const lta::SurfaceTracer surfaceTracer{m_scene};
	const lta::RussianRoulette rr{};

	const PrimitiveMetadata* metadata      = X.getDetail().getPrimitive()->getMetadata();
	const SurfaceOptics*     surfaceOptics = metadata->getSurface().getOptics();

	const SurfacePhenomena targetPhenomena = policy.getTargetPhenomena();
	for(SurfaceElemental i = 0; i < surfaceOptics->numElementals(); ++i)
	{
		if(targetPhenomena.hasNone({surfaceOptics->getPhenomenonOf(i)}))
		{
			continue;
		}

		BsdfSampleQuery elementalSample(BsdfQueryContext(i, transport, sidednessPolicy));
		elementalSample.inputs.set(X, V);

		Ray sampledRay;
		if(!surfaceTracer.doBsdfSample(elementalSample, sampleFlow, &sampledRay))
		{
			continue;
		}

		math::Spectrum elementalPathThroughput(pathThroughput);
		elementalPathThroughput *= elementalSample.outputs.getPdfAppliedBsdfCos();

		// Prevent premature termination of the path due to solid angle compression/expansion
		rrScale /= elementalSample.outputs.getRelativeIor2();

		if(policy.useRussianRoulette())
		{
			real rrSurvivalProb;
			if(rr.surviveOnLuminance(elementalPathThroughput * rrScale, sampleFlow, &rrSurvivalProb))
			{
				elementalPathThroughput *= 1.0_r / rrSurvivalProb;
			}
			else
			{
				continue;
			}
		}

		traceViewPath(
			X,
			sampledRay,
			elementalPathThroughput,
			pathLength,
			sampleFlow,
			rrScale);
	}// end for each phenomenon
}

}// end namespace ph
