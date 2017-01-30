#include "Core/Integrator/NormalBufferIntegrator.h"
#include "Core/Ray.h"
#include "Core/Intersection.h"
#include "World/World.h"
#include "World/Intersector.h"
#include "Math/TVector3.h"

namespace ph
{

NormalBufferIntegrator::~NormalBufferIntegrator() = default;

void NormalBufferIntegrator::update(const World& world)
{
	// update nothing
}

void NormalBufferIntegrator::radianceAlongRay(const Sample& sample, const World& world, const Camera& camera, std::vector<SenseEvent>& out_senseEvents) const
{
	Ray ray;
	camera.genSensingRay(sample, &ray);

	// reverse tracing
	const Ray tracingRay(ray.getOrigin(), ray.getDirection().mul(-1.0f), RAY_T_EPSILON, RAY_T_MAX);
	
	Vector3R radiance;
	Intersection intersection;
	if(world.getIntersector().isIntersecting(tracingRay, &intersection))
	{
		radiance = intersection.getHitSmoothNormal();
	}
	else
	{
		radiance = Vector3R(0, 0, 0);
	}

	out_senseEvents.push_back(SenseEvent(sample.m_cameraX, sample.m_cameraY, radiance));
}

}// end namespace ph