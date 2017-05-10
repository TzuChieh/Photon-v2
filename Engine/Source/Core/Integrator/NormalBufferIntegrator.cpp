#include "Core/Integrator/NormalBufferIntegrator.h"
#include "Core/Ray.h"
#include "Core/Intersection.h"
#include "World/Scene.h"
#include "Math/TVector3.h"

namespace ph
{

NormalBufferIntegrator::~NormalBufferIntegrator() = default;

void NormalBufferIntegrator::update(const Scene& scene)
{
	// update nothing
}

void NormalBufferIntegrator::radianceAlongRay(const Sample& sample, const Scene& scene, const Camera& camera, std::vector<SenseEvent>& out_senseEvents) const
{
	Ray ray;
	camera.genSensingRay(sample, &ray);

	// reverse tracing
	const Ray tracingRay(ray.getOrigin(), ray.getDirection().mul(-1.0f), 0.0001_r, Ray::MAX_T);// HACK: hard-coded number
	
	Vector3R radiance;
	Intersection intersection;
	if(scene.isIntersecting(tracingRay, &intersection))
	{
		radiance = intersection.getHitSmoothNormal();
	}
	else
	{
		radiance = Vector3R(0, 0, 0);
	}

	out_senseEvents.push_back(SenseEvent(sample.m_cameraX, sample.m_cameraY, radiance));
}

// command interface

NormalBufferIntegrator::NormalBufferIntegrator(const InputPacket& packet) :
	Integrator(packet)
{

}

SdlTypeInfo NormalBufferIntegrator::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_INTEGRATOR, "surface-normal");
}

ExitStatus NormalBufferIntegrator::ciExecute(const std::shared_ptr<NormalBufferIntegrator>& targetResource, const std::string& functionName, const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph