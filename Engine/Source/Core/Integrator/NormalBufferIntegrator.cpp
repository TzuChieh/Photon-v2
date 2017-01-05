#include "Core/Integrator/NormalBufferIntegrator.h"
#include "Core/Ray.h"
#include "Core/Intersection.h"
#include "World/World.h"
#include "World/Intersector.h"

namespace ph
{

NormalBufferIntegrator::~NormalBufferIntegrator() = default;

void NormalBufferIntegrator::update(const World& world)
{
	// update nothing
}

void NormalBufferIntegrator::radianceAlongRay(const Ray& ray, const World& world, Vector3f* const out_radiance) const
{
	// reverse tracing
	const Ray tracingRay(ray.getOrigin(), ray.getDirection().mul(-1.0f), RAY_T_EPSILON, RAY_T_MAX);
	
	Intersection intersection;
	if(world.getIntersector().isIntersecting(tracingRay, &intersection))
	{
		*out_radiance = intersection.getHitSmoothNormal();
	}
	else
	{
		*out_radiance = Vector3f(0, 0, 0);
	}
}

}// end namespace ph