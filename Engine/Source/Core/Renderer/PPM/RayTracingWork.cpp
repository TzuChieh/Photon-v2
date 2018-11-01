#include "Core/Renderer/PPM/RayTracingWork.h"
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

namespace ph
{

RayTracingWork::RayTracingWork(
	const Scene* const scene,
	const Camera* const camera,
	std::unique_ptr<SampleGenerator> sampleGenerator,
	Viewpoint* const viewpointBuffer, 
	const std::size_t numViewpoints) : 
	m_scene(scene),
	m_camera(camera),
	m_sampleGenerator(std::move(sampleGenerator)),
	m_viewpointBuffer(viewpointBuffer), 
	m_numViewpoints(numViewpoints)
{}

void RayTracingWork::doWork()
{
	const Samples2DStage filmStage = m_sampleGenerator->declare2DStage(m_numViewpoints);// FIXME: size hints
	const bool isSamplePrepared = m_sampleGenerator->prepareSampleBatch();
	PH_ASSERT(isSamplePrepared);
	
	const Samples2D samples = m_sampleGenerator->getSamples2D(filmStage);
	for(std::size_t i = 0; i < samples.numSamples(); ++i)
	{
		const Vector2R filmNdcPos = samples[i];

		Ray tracingRay;
		m_camera->genSensedRay(filmNdcPos, &tracingRay);
		tracingRay.reverse();

		HitProbe probe;
		if(m_scene->isIntersecting(tracingRay, &probe))
		{
			SurfaceHit surfaceHit(tracingRay, probe);
			const PrimitiveMetadata* metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
			const SurfaceOptics* surfaceOptics = metadata->getSurface().getOptics();

			// TODO: handle specular path

			Viewpoint viewpoint;
			viewpoint.filmNdcPos = filmNdcPos;
			viewpoint.hit = surfaceHit.getDetail();
			viewpoint.numPhotons = 0;
			viewpoint.radius = 0.1_r;
			viewpoint.throughput = SpectralStrength(1.0_r);
			viewpoint.V = tracingRay.getDirection().mul(-1);
			m_viewpointBuffer[i] = viewpoint;
		}
	}
}

}// end namespace ph