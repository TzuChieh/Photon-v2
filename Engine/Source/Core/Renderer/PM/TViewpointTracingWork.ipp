#pragma once

#include "Core/Renderer/PM/TViewpointGatheringWork.h"
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

namespace ph
{

template<typename Viewpoint>
inline TViewpointTracingWork<Viewpoint>::TViewpointTracingWork(
	const Scene* const scene,
	const Camera* const camera,
	SampleGenerator* sampleGenerator,
	const Region& filmRegion,
	const real kernelRadius) : 
	m_scene(scene),
	m_camera(camera),
	m_sampleGenerator(std::move(sampleGenerator)),
	m_kernelRadius(kernelRadius),
	m_viewpoints(),
	m_filmRegion(filmRegion),
	m_maxViewpointDepth(6)// TODO: expose this parameter
{}

template<typename Viewpoint>
inline void TViewpointTracingWork<Viewpoint>::doWork()
{
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

			gatherViewpointsRecursive(tracingRay, filmNdc, SpectralStrength(1), 1);
		}
	}
}

template<typename Viewpoint>
inline void TViewpointTracingWork<Viewpoint>::gatherViewpointsRecursive(
	const Ray& tracingRay, 
	const Vector2R& filmNdc,
	const SpectralStrength& throughput,
	const std::size_t currentViewpointDepth)
{
	if(currentViewpointDepth > m_maxViewpointDepth)
	{
		PH_ASSERT_EQ(currentViewpointDepth, m_maxViewpointDepth + 1);
		return;
	}

	TSurfaceEventDispatcher<ESaPolicy::STRICT> surfaceEvent(m_scene);

	SurfaceHit surfaceHit;
	if(surfaceEvent.traceNextSurface(tracingRay, &surfaceHit))
	{
		const PrimitiveMetadata* metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
		const SurfaceOptics* surfaceOptics = metadata->getSurface().getOptics();

		const Vector3R V = tracingRay.getDirection().mul(-1);
		const Vector3R N = surfaceHit.getShadingNormal();

		for(SurfaceElemental i = 0; i < surfaceOptics->numElementals(); ++i)
		{
			if(surfaceOptics->getPhenomenaOf(i).hasAtLeastOne({
				ESP::DELTA_REFLECTION, 
				ESP::DELTA_TRANSMISSION}))
			{
				BsdfSample sample;
				sample.inputs.set(surfaceHit, V, i, ETransport::RADIANCE);

				Ray sampledRay;
				if(surfaceEvent.doBsdfSample(surfaceHit, sample, &sampledRay))
				{
					// TODO: what about emitter with delta BSDF? cannot capture its radiance here

					gatherViewpointsRecursive(
						sampledRay, 
						filmNdc, 
						throughput.mul(sample.outputs.pdfAppliedBsdf).mul(N.absDot(sampledRay.getDirection())),
						currentViewpointDepth + 1);
				}
			}
			else
			{
				Viewpoint viewpoint;

				if constexpr(Viewpoint::template has<EViewpointData::SURFACE_HIT>()) {
					viewpoint.template set<EViewpointData::SURFACE_HIT>(surfaceHit);
				}
				if constexpr(Viewpoint::template has<EViewpointData::FILM_NDC>()) {
					viewpoint.template set<EViewpointData::FILM_NDC>(filmNdc);
				}
				if constexpr(Viewpoint::template has<EViewpointData::RADIUS>()) {
					viewpoint.template set<EViewpointData::RADIUS>(m_kernelRadius);
				}
				if constexpr(Viewpoint::template has<EViewpointData::NUM_PHOTONS>()) {
					viewpoint.template set<EViewpointData::NUM_PHOTONS>(0.0_r);
				}
				if constexpr(Viewpoint::template has<EViewpointData::TAU>()) {
					viewpoint.template set<EViewpointData::TAU>(SpectralStrength(0));
				}
				if constexpr(Viewpoint::template has<EViewpointData::VIEW_THROUGHPUT>()) {
					viewpoint.template set<EViewpointData::VIEW_THROUGHPUT>(throughput);
				}
				if constexpr(Viewpoint::template has<EViewpointData::VIEW_DIR>()) {
					viewpoint.template set<EViewpointData::VIEW_DIR>(V);
				}

				if constexpr(Viewpoint::template has<EViewpointData::VIEW_RADIANCE>())
				{
					SpectralStrength surfaceRadiance(0);
					if(metadata->getSurface().getEmitter())
					{
						metadata->getSurface().getEmitter()->evalEmittedRadiance(surfaceHit, &surfaceRadiance);
						surfaceRadiance.mulLocal(throughput);
					}
					viewpoint.template set<EViewpointData::VIEW_RADIANCE>(surfaceRadiance);
				}

				m_viewpoints.push_back(viewpoint);
			}
		}// end for each phenomenon
	}
}

}// end namespace ph