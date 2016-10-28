#include "Core/PathTracer.h"
#include "Common/primitive_type.h"
#include "Frame/HDRFrame.h"
#include "Core/World.h"
#include "Camera/Camera.h"
#include "Core/Ray.h"
#include "Core/Intersection.h"
#include "Model/Material/Material.h"
#include "Model/Material/SurfaceIntegrand.h"

namespace ph
{

void PathTracer::trace(const Camera& camera, const World& world, HDRFrame* const out_hdrFrame) const
{
	const uint32 widthPx = out_hdrFrame->getWidthPx();
	const uint32 heightPx = out_hdrFrame->getHeightPx();

	for(uint32 y = 0; y < heightPx; y++)
	{
		for(uint32 x = 0; x < widthPx; x++)
		{
			Vector3f accuRadiance(0, 0, 0);
			const uint32 spp = 16;
			uint32 numSamples = 0;
			
			while(numSamples < spp)
			{
				Ray ray;
				Intersection intersection;
				const uint32 maxBounces = 5;
				uint32 numBounces = 0;

				camera.genSampleRay(&ray, widthPx, heightPx, x, y);

				while(numBounces <= maxBounces)
				{
					if(world.isIntersecting(ray, &intersection))
					{
						const Model* hitModel = intersection.getHitPrimitive()->getParentModel();
						const Material* hitMaterial = hitModel->getMaterial();

						Vector3f L;
						Vector3f N(intersection.getHitNormal());
						Vector3f V(camera.getPosition().sub(intersection.getHitPosition()));
						V.normalizeLocal();

						hitMaterial->getSurfaceIntegrand()->genUniformRandomLOverRegion(N, &L);

						if(!hitMaterial->getSurfaceIntegrand()->sampleLiWeight(L, V, N, ray))
						{
							Vector3f radiance;
							ray.calcWeightedLiRadiance(&radiance);
							accuRadiance.addLocal(radiance);
							
							break;
						}
						else
						{
							Vector3f nextRayOrigin(intersection.getHitPosition().add(N.mul(0.0001f)));
							Vector3f nextRayDirection(L);
							ray.setOrigin(nextRayOrigin);
							ray.setDirection(nextRayDirection);
						}

						numBounces++;
					}
					else
					{
						break;
					}
				}// end while

				numSamples++;
			}// end while

			out_hdrFrame->setPixel(x, y, accuRadiance.x / static_cast<float32>(spp));
		}
	}
}

}// end namespace ph