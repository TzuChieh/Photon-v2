#include "Core/ImportanceRenderer.h"
#include "Common/primitive_type.h"
#include "Frame/HDRFrame.h"
#include "Core/World.h"
#include "Camera/Camera.h"
#include "Core/Ray.h"
#include "Core/Intersection.h"
#include "Model/Material/Material.h"
#include "Model/Material/SurfaceIntegrand.h"
#include "Math/constant.h"

#include <cmath>
#include <algorithm>
#include <iostream>

namespace ph
{

ImportanceRenderer::~ImportanceRenderer() = default;

void ImportanceRenderer::render(const World& world, const Camera& camera, HDRFrame* const out_frame) const
{
	const uint32 widthPx = out_frame->getWidthPx();
	const uint32 heightPx = out_frame->getHeightPx();

	for(uint32 y = 0; y < heightPx; y++)
	{
		for(uint32 x = 0; x < widthPx; x++)
		{
			Vector3f accuRadiance(0, 0, 0);
			const uint32 spp = 64;
			uint32 numSamples = 0;

			while(numSamples < spp)
			{
				Ray ray;
				Intersection intersection;
				const uint32 maxBounces = 5;
				uint32 numBounces = 0;

				camera.genSampleRay(&ray, widthPx, heightPx, x, y);

				while(numBounces <= maxBounces && world.isIntersecting(ray, &intersection))
				{
					const Model* hitModel = intersection.getHitPrimitive()->getParentModel();
					const Material* hitMaterial = hitModel->getMaterial();

					Vector3f L;
					Vector3f N(intersection.getHitNormal());
					Vector3f V(ray.getDirection().mul(-1.0f));

					hitMaterial->getSurfaceIntegrand()->genUniformRandomLOverRegion(N, &L);

					if(hitMaterial->getSurfaceIntegrand()->isEmissive())
					{
						Vector3f radiance;
						hitMaterial->getSurfaceIntegrand()->sampleEmittedRadiance(intersection, L, V, &radiance);

						ray.addLiRadiance(radiance);
						ray.calcWeightedLiRadiance(&radiance);
						accuRadiance.addLocal(radiance);

						break;
					}

					Vector3f BRDF;
					hitMaterial->getSurfaceIntegrand()->sampleBRDF(intersection, L, V, &BRDF);

					//ray.accumulateLiWeight(BRDF.mulLocal(2.0f * PI_FLOAT32 * std::max(N.dot(L), 0.0f)));
					ray.accumulateLiWeight(BRDF.mulLocal(2.0f * PI_FLOAT32 * N.dot(L)));
					if(N.dot(L) < 0.0f)
					{
						std::cout << N.dot(L) << std::endl;
					}

					Vector3f nextRayOrigin(intersection.getHitPosition().add(N.mul(0.0001f)));
					Vector3f nextRayDirection(L);
					ray.setOrigin(nextRayOrigin);
					ray.setDirection(nextRayDirection);

					numBounces++;
				}// end while

				numSamples++;
			}// end while

			accuRadiance.divLocal(static_cast<float32>(spp));
			out_frame->setPixel(x, y, accuRadiance.x, accuRadiance.y, accuRadiance.z);
		}
	}
}

}// end namespace ph