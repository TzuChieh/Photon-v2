#pragma once

#include "Core/Renderer/PM/TViewPathTracingWork.h"
#include "World/Scene.h"
#include "Core/Receiver/Receiver.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Ray.h"
#include "Core/HitDetail.h"
#include "Core/HitProbe.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/LTABuildingBlock/SurfaceTracer.h"
#include "Core/LTABuildingBlock/RussianRoulette.h"
#include "Math/Geometry/TAABB2D.h"
#include "Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Math/Random/sample.h"

#include <Common/assertion.h>

namespace ph
{

template<typename ViewPathHandler>
inline TViewPathTracingWork<ViewPathHandler>::TViewPathTracingWork(

	ViewPathHandler* const handler,
	const Scene* const scene,
	const Receiver* const receiver,
	SampleGenerator* sampleGenerator,
	const Region& rasterRegion) :

	m_handler(handler),
	m_scene(scene),
	m_receiver(receiver),
	m_sampleGenerator(sampleGenerator),
	m_rasterRegion(rasterRegion)
{}

template<typename ViewPathHandler>
inline void TViewPathTracingWork<ViewPathHandler>::doWork()
{
	PH_ASSERT(m_handler);

	const auto rasterSampleHandle = m_sampleGenerator->declareStageND(
		2,
		m_rasterRegion.getArea(),
		{static_cast<std::size_t>(m_rasterRegion.getWidth()), static_cast<std::size_t>(m_rasterRegion.getHeight())});

	const auto raySampleHandle = m_sampleGenerator->declareStageND(2, m_rasterRegion.getArea());

	const math::TAABB2D<float64> rRegion(m_rasterRegion);

	while(m_sampleGenerator->prepareSampleBatch())
	{
		auto rasterSamples = m_sampleGenerator->getSamplesND(rasterSampleHandle);
		auto raySamples = m_sampleGenerator->getSamplesND(raySampleHandle);
		for(std::size_t i = 0; i < rasterSamples.numSamples(); ++i)
		{
			// TODO: use TArithmeticArray directly
			
			const auto rasterCoord = rRegion.sampleToSurface(math::sample_cast<float64>(rasterSamples.template get<2>(i)));
			SampleFlow sampleFlow = raySamples.readSampleAsFlow();

			Ray tracingRay;
			const auto quantityWeight = m_receiver->receiveRay(rasterCoord, &tracingRay);
			tracingRay.reverse();

			math::Spectrum pathThroughput(quantityWeight);
			if(!m_handler->onReceiverSampleStart(rasterCoord, pathThroughput))
			{
				m_handler->onReceiverSampleEnd();
				continue;
			}

			traceViewPath(
				tracingRay, 
				pathThroughput, 
				0,
				sampleFlow);
			
			m_handler->onReceiverSampleEnd();
		}// end for single sample

		m_handler->onSampleBatchFinished();
	}// end while single sample batch
}

template<typename ViewPathHandler>
inline void TViewPathTracingWork<ViewPathHandler>::traceViewPath(
	Ray            tracingRay,
	math::Spectrum pathThroughput,
	std::size_t    pathLength,
	SampleFlow&    sampleFlow)
{	
	const SurfaceTracer surfaceTracer(m_scene);
	while(true)
	{
		SurfaceHit surfaceHit;
		if(!surfaceTracer.traceNextSurface(tracingRay, BsdfQueryContext().sidedness, &surfaceHit))
		{
			break;
		}

		++pathLength;
		const ViewPathTracingPolicy& policy = m_handler->onPathHitSurface(pathLength, surfaceHit, pathThroughput);
		if(policy.isKilled())
		{
			break;
		}

		// FIXME: reversed then assigned again later seems to be dangerous, state is unclear
		tracingRay.reverse();
		const math::Vector3R V = tracingRay.getDirection();
		const math::Vector3R N = surfaceHit.getShadingNormal();

		if(policy.getSampleMode() == EViewPathSampleMode::SINGLE_PATH)
		{
			BsdfSampleQuery bsdfSample(BsdfQueryContext(policy.getTargetElemental(), ETransport::RADIANCE, ESidednessPolicy::STRICT));
			bsdfSample.inputs.set(surfaceHit, V);
			Ray sampledRay;
			if(!surfaceTracer.doBsdfSample(bsdfSample, sampleFlow, &sampledRay))
			{
				break;
			}

			pathThroughput.mulLocal(bsdfSample.outputs.pdfAppliedBsdf);
			pathThroughput.mulLocal(N.absDot(bsdfSample.outputs.L));

			if(policy.useRussianRoulette())
			{
				math::Spectrum weightedThroughput;
				if(RussianRoulette::surviveOnLuminance(pathThroughput, sampleFlow, &weightedThroughput))
				{
					pathThroughput = weightedThroughput;
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
			traceElementallyBranchedPath(policy, V, N, surfaceHit, pathThroughput, pathLength, sampleFlow);
			break;
		}
	}// end while true
}

template<typename ViewPathHandler>
inline void TViewPathTracingWork<ViewPathHandler>::traceElementallyBranchedPath(
	const ViewPathTracingPolicy& policy,
	const math::Vector3R& V,
	const math::Vector3R& N,
	const SurfaceHit& surfaceHit,
	const math::Spectrum& pathThroughput,
	const std::size_t pathLength,
	SampleFlow& sampleFlow)
{
	PH_ASSERT(policy.getSampleMode() == EViewPathSampleMode::ELEMENTAL_BRANCH);

	const SurfaceTracer surfaceTracer(m_scene);

	const PrimitiveMetadata* metadata      = surfaceHit.getDetail().getPrimitive()->getMetadata();
	const SurfaceOptics*     surfaceOptics = metadata->getSurface().getOptics();

	const SurfacePhenomena targetPhenomena = policy.getTargetPhenomena();
	for(SurfaceElemental i = 0; i < surfaceOptics->numElementals(); ++i)
	{
		if(targetPhenomena.hasNone({surfaceOptics->getPhenomenonOf(i)}))
		{
			continue;
		}

		BsdfSampleQuery sample(BsdfQueryContext(i, ETransport::RADIANCE, ESidednessPolicy::STRICT));
		sample.inputs.set(surfaceHit, V);

		Ray sampledRay;
		if(!surfaceTracer.doBsdfSample(sample, sampleFlow, &sampledRay))
		{
			continue;
		}

		math::Spectrum elementalPathThroughput(pathThroughput);
		elementalPathThroughput.mulLocal(sample.outputs.pdfAppliedBsdf);
		elementalPathThroughput.mulLocal(N.absDot(sampledRay.getDirection()));

		if(policy.useRussianRoulette())
		{
			math::Spectrum weightedThroughput;
			if(RussianRoulette::surviveOnLuminance(elementalPathThroughput, sampleFlow, &weightedThroughput))
			{
				elementalPathThroughput = weightedThroughput;
			}
			else
			{
				continue;
			}
		}

		traceViewPath(
			sampledRay,
			elementalPathThroughput,
			pathLength,
			sampleFlow);
	}// end for each phenomenon
}

}// end namespace ph
