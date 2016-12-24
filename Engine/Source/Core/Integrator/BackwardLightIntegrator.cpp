#include "Core/Integrator/BackwardLightIntegrator.h"
#include "Core/Ray.h"
#include "World/World.h"
#include "World/Intersector.h"
#include "World/LightSampler/LightSampler.h"
#include "Math/Vector3f.h"
#include "Core/Intersection.h"
#include "Entity/Material/SurfaceBehavior/SurfaceSample.h"
#include "Entity/Material/Material.h"
#include "Entity/Material/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/Primitive/Primitive.h"
#include "Core/Primitive/PrimitiveMetadata.h"

#define MAX_RAY_BOUNCES 10000

namespace ph
{

BackwardLightIntegrator::~BackwardLightIntegrator() = default;

void BackwardLightIntegrator::update(const World& world)
{
	// update nothing
}

void BackwardLightIntegrator::radianceAlongRay(const Ray& ray, const World& world, Vector3f* const out_radiance) const
{
	const Intersector& intersector = world.getIntersector();
	const LightSampler& lightSampler = world.getLightSampler();

	// reverse ray for backward tracing
	Ray tracingRay(ray.getOrigin(), ray.getDirection().mul(-1.0f), RAY_T_EPSILON, RAY_T_MAX);
	Intersection intersection;
	Vector3f accuRadiance(0, 0, 0);
	Vector3f accuLiWeight(1, 1, 1);

	for(uint32 numBounces = 0; numBounces < MAX_RAY_BOUNCES; numBounces++)
	{
		if(!intersector.isIntersecting(tracingRay, &intersection))
		{
			break;
		}

		SurfaceSample surfaceSample;
		const PrimitiveMetadata* metadata = intersection.getHitPrimitive()->getMetadata();
		const Material* hitMaterial = metadata->m_material;
		const SurfaceBehavior* surfaceBehavior = hitMaterial->getSurfaceBehavior();

		///////////////////////////////////////////////////////////////////////////////
		// direct light sample

		float32 emitterPickPDF;
		const Emitter* emitter = lightSampler.pickEmitter(&emitterPickPDF);
		if(emitter)
		{
			
		}

		///////////////////////////////////////////////////////////////////////////////
		// indirect light sample
	}

	*out_radiance = accuRadiance;
}

}// end namespace ph