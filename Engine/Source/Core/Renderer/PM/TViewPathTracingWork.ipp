#pragma once

#include "Core/Renderer/PM/TViewPathTracingWork.h"
#include "World/Scene.h"
#include "Core/Camera/Camera.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Common/assertion.h"
#include "Core/Ray.h"
#include "Core/HitDetail.h"
#include "Core/HitProbe.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/LTABuildingBlock/TSurfaceEventDispatcher.h"
#include "Core/LTABuildingBlock/RussianRoulette.h"

namespace ph
{

template<typename ViewPathHandler>
inline TViewPathTracingWork<ViewPathHandler>::TViewPathTracingWork(
	ViewPathHandler* const handler,
	const Scene* const scene,
	const Camera* const camera,
	SampleGenerator* sampleGenerator,
	const Region& filmRegion) : 
	m_handler(handler),
	m_scene(scene),
	m_camera(camera),
	m_sampleGenerator(std::move(sampleGenerator)),
	m_filmRegion(filmRegion)
{}

template<typename ViewPathHandler>
inline void TViewPathTracingWork<ViewPathHandler>::doWork()
{
	PH_ASSERT(m_handler);

	const Samples2DStage filmStage = m_sampleGenerator->declare2DStage(m_filmRegion.calcArea());// FIXME: size hints
	while(m_sampleGenerator->prepareSampleBatch())
	{
		const Samples2D samples = m_sampleGenerator->getSamples2D(filmStage);
		for(std::size_t i = 0; i < samples.numSamples(); ++i)
		{
			const Vector2R filmNdc = samples[i];

			Ray tracingRay;
			m_camera->genSensedRay(filmNdc, &tracingRay);
			tracingRay.reverse();

			const std::size_t pathLength = 0;
			SpectralStrength  pathThroughput(1);// FIXME: camera might affect initial throughput
			if(!m_handler->onCameraSampleStart(filmNdc, pathThroughput))
			{
				m_handler->onCameraSampleEnd();
				continue;
			}
			
			traceViewPath(
				tracingRay, 
				pathThroughput, 
				pathLength);
			
			m_handler->onCameraSampleEnd();
		}// end for single sample

		m_handler->onSampleBatchFinished();
	}// end while single sample batch
}

template<typename ViewPathHandler>
inline void TViewPathTracingWork<ViewPathHandler>::traceViewPath(
	Ray              tracingRay,
	SpectralStrength pathThroughput,
	std::size_t      pathLength)
{	
	TSurfaceEventDispatcher<ESaPolicy::STRICT> surfaceEvent(m_scene);
	while(true)
	{
		PH_ASSERT_LT(pathLength, m_maxPathLength);

		SurfaceHit surfaceHit;
		if(!surfaceEvent.traceNextSurface(tracingRay, &surfaceHit))
		{
			break;
		}

		++pathLength;
		const ViewPathTracingPolicy& policy = m_handler->onPathHitSurface(pathLength, surfaceHit, pathThroughput);
		if(policy.isKilled() || pathLength == m_maxPathLength)
		{
			break;
		}

		const Vector3R V = tracingRay.getDirection().mulLocal(-1);
		const Vector3R N = surfaceHit.getShadingNormal();

		if(policy.getSampleMode() == EViewPathSampleMode::SINGLE_PATH)
		{
			BsdfSample bsdfSample;
			Ray sampledRay;
			bsdfSample.inputs.set(surfaceHit, V, ALL_ELEMENTALS, ETransport::RADIANCE);
			if(!surfaceEvent.doBsdfSample(surfaceHit, bsdfSample, &sampledRay))
			{
				break;
			}

			pathThroughput.mulLocal(bsdfSample.outputs.pdfAppliedBsdf);
			pathThroughput.mulLocal(N.absDot(bsdfSample.outputs.L));

			if(policy.useRussianRoulette())
			{
				SpectralStrength weightedThroughput;
				if(RussianRoulette::surviveOnLuminance(pathThroughput, &weightedThroughput))
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
			traceElementallyBranchedPath(policy, V, N, surfaceHit, pathThroughput, pathLength);
			break;
		}
	}// end while true
}

template<typename ViewPathHandler>
inline void TViewPathTracingWork<ViewPathHandler>::traceElementallyBranchedPath(
	const ViewPathTracingPolicy& policy,
	const Vector3R& V,
	const Vector3R& N,
	const SurfaceHit& surfaceHit,
	const SpectralStrength& pathThroughput,
	const std::size_t pathLength)
{
	PH_ASSERT_EQ(policy.getSampleMode(), EViewPathSampleMode::ELEMENTAL_BRANCH);

	TSurfaceEventDispatcher<ESaPolicy::STRICT> surfaceEvent(m_scene);

	const PrimitiveMetadata* metadata      = surfaceHit.getDetail().getPrimitive()->getMetadata();
	const SurfaceOptics*     surfaceOptics = metadata->getSurface().getOptics();

	const SurfacePhenomena targetPhenomena = policy.getTargetPhenomena();
	for(SurfaceElemental i = 0; i < surfaceOptics->numElementals(); ++i)
	{
		if(targetPhenomena.hasNone(SurfaceElemental(surfaceOptics->getPhenomenonOf(i))))
		{
			continue;
		}

		BsdfSample sample;
		sample.inputs.set(surfaceHit, V, i, ETransport::RADIANCE);

		Ray sampledRay;
		if(!surfaceEvent.doBsdfSample(surfaceHit, sample, &sampledRay))
		{
			continue;
		}

		SpectralStrength elementalPathThroughput(pathThroughput);
		elementalPathThroughput.mulLocal(sample.outputs.pdfAppliedBsdf);
		elementalPathThroughput.mulLocal(N.absDot(sampledRay.getDirection()));

		if(policy.useRussianRoulette())
		{
			SpectralStrength weightedThroughput;
			if(RussianRoulette::surviveOnLuminance(elementalPathThroughput, &weightedThroughput))
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
			pathLength);
	}// end for each phenomenon
}

//template<typename Viewpoint>
//inline void TViewpointTracingWork<Viewpoint>::doWork()
//{
//	const Samples2DStage filmStage = m_sampleGenerator->declare2DStage(m_filmRegion.calcArea());// FIXME: size hints
//	while(m_sampleGenerator->prepareSampleBatch())
//	{
//		const Samples2D samples = m_sampleGenerator->getSamples2D(filmStage);
//		for(std::size_t i = 0; i < samples.numSamples(); ++i)
//		{
//			const Vector2R filmNdc = samples[i];
//
//			Ray tracingRay;
//			m_camera->genSensedRay(filmNdc, &tracingRay);
//			tracingRay.reverse();
//
//			gatherViewpointsRecursive(tracingRay, filmNdc, SpectralStrength(1), 1);
//		}
//	}
//}
//
//template<typename Viewpoint>
//inline void TViewpointTracingWork<Viewpoint>::gatherViewpointsRecursive(
//	const Ray& tracingRay, 
//	const Vector2R& filmNdc,
//	const SpectralStrength& throughput,
//	const std::size_t currentViewpointDepth)
//{
//	if(currentViewpointDepth > m_maxViewpointDepth)
//	{
//		PH_ASSERT_EQ(currentViewpointDepth, m_maxViewpointDepth + 1);
//		return;
//	}
//
//	TSurfaceEventDispatcher<ESaPolicy::STRICT> surfaceEvent(m_scene);
//
//	SurfaceHit surfaceHit;
//	if(surfaceEvent.traceNextSurface(tracingRay, &surfaceHit))
//	{
//		const PrimitiveMetadata* metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
//		const SurfaceOptics* surfaceOptics = metadata->getSurface().getOptics();
//
//		const Vector3R V = tracingRay.getDirection().mul(-1);
//		const Vector3R N = surfaceHit.getShadingNormal();
//
//		for(SurfaceElemental i = 0; i < surfaceOptics->numElementals(); ++i)
//		{
//			if(surfaceOptics->getPhenomenaOf(i).hasAtLeastOne({
//				ESP::DELTA_REFLECTION, 
//				ESP::DELTA_TRANSMISSION}))
//			{
//				BsdfSample sample;
//				sample.inputs.set(surfaceHit, V, i, ETransport::RADIANCE);
//
//				Ray sampledRay;
//				if(surfaceEvent.doBsdfSample(surfaceHit, sample, &sampledRay))
//				{
//					// TODO: what about emitter with delta BSDF? cannot capture its radiance here
//
//					gatherViewpointsRecursive(
//						sampledRay, 
//						filmNdc, 
//						throughput.mul(sample.outputs.pdfAppliedBsdf).mul(N.absDot(sampledRay.getDirection())),
//						currentViewpointDepth + 1);
//				}
//			}
//			else
//			{
//				Viewpoint viewpoint;
//
//				if constexpr(Viewpoint::template has<EViewpointData::SURFACE_HIT>()) {
//					viewpoint.template set<EViewpointData::SURFACE_HIT>(surfaceHit);
//				}
//				if constexpr(Viewpoint::template has<EViewpointData::FILM_NDC>()) {
//					viewpoint.template set<EViewpointData::FILM_NDC>(filmNdc);
//				}
//				if constexpr(Viewpoint::template has<EViewpointData::RADIUS>()) {
//					viewpoint.template set<EViewpointData::RADIUS>(m_kernelRadius);
//				}
//				if constexpr(Viewpoint::template has<EViewpointData::NUM_PHOTONS>()) {
//					viewpoint.template set<EViewpointData::NUM_PHOTONS>(0.0_r);
//				}
//				if constexpr(Viewpoint::template has<EViewpointData::TAU>()) {
//					viewpoint.template set<EViewpointData::TAU>(SpectralStrength(0));
//				}
//				if constexpr(Viewpoint::template has<EViewpointData::VIEW_THROUGHPUT>()) {
//					viewpoint.template set<EViewpointData::VIEW_THROUGHPUT>(throughput);
//				}
//				if constexpr(Viewpoint::template has<EViewpointData::VIEW_DIR>()) {
//					viewpoint.template set<EViewpointData::VIEW_DIR>(V);
//				}
//
//				if constexpr(Viewpoint::template has<EViewpointData::VIEW_RADIANCE>())
//				{
//					SpectralStrength surfaceRadiance(0);
//					if(metadata->getSurface().getEmitter())
//					{
//						metadata->getSurface().getEmitter()->evalEmittedRadiance(surfaceHit, &surfaceRadiance);
//						surfaceRadiance.mulLocal(throughput);
//					}
//					viewpoint.template set<EViewpointData::VIEW_RADIANCE>(surfaceRadiance);
//				}
//
//				m_viewpoints.push_back(viewpoint);
//			}
//		}// end for each phenomenon
//	}
//}

}// end namespace ph