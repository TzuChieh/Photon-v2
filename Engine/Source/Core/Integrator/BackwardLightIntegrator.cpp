#include "Core/Integrator/BackwardLightIntegrator.h"
#include "Core/Ray.h"
#include "World/Intersector.h"
#include "Math/Vector3f.h"
#include "Core/Intersection.h"
#include "Entity/Material/Integrand/SurfaceSample.h"
#include "Entity/Material/Material.h"
#include "Entity/Material/Integrand/SurfaceIntegrand.h"
#include "Entity/Primitive/Primitive.h"
#include "Entity/Primitive/PrimitiveMetadata.h"

#define MAX_RAY_BOUNCES 10000

namespace ph
{

BackwardLightIntegrator::~BackwardLightIntegrator() = default;

void BackwardLightIntegrator::update(const Intersector& intersector)
{
	// update nothing
}

void BackwardLightIntegrator::radianceAlongRay(const Ray& ray, const Intersector& intersector, Vector3f* const out_radiance) const
{
	Ray tracingRay(ray.getOrigin(), ray.getDirection().mul(-1.0f));// reverse ray for backward tracing
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
		const SurfaceIntegrand* surfaceIntegrand = hitMaterial->getSurfaceIntegrand();

		// TODO: sample light

		// TODO: sample indirect light
	}

	*out_radiance = accuRadiance;
}

}// end namespace ph