#include "Core/ImportanceRenderer.h"
#include "Common/primitive_type.h"
#include "Image/HDRFrame.h"
#include "Core/World.h"
#include "Camera/Camera.h"
#include "Core/Ray.h"
#include "Core/Intersection.h"
#include "Model/Material/Material.h"
#include "Model/Material/SurfaceIntegrand.h"
#include "Math/constant.h"
#include "Core/StandardSampleGenerator.h"
#include "Core/Sample.h"

#include <cmath>
#include <iostream>
#include <vector>

namespace ph
{

ImportanceRenderer::~ImportanceRenderer() = default;

void ImportanceRenderer::render(const World& world, const Camera& camera, HDRFrame* const out_frame) const
{
	const uint32 widthPx = out_frame->getWidthPx();
	const uint32 heightPx = out_frame->getHeightPx();

	const float32 aspectRatio = static_cast<float32>(widthPx) / static_cast<float32>(heightPx);

	const uint32 spp = 16;
	const uint32 maxBounces = 5;
	StandardSampleGenerator sampleGenerator(spp);
	std::vector<Sample> samples;


	int32 numSpp = 0;

	while(sampleGenerator.hasMoreSamples())
	{
		samples.clear();
		sampleGenerator.requestMoreSamples(*out_frame, &samples);

		Sample sample;
		while(!samples.empty())
		{
			uint32 numBounces = 0;
			Vector3f accuRadiance(0, 0, 0);
			Ray ray;
			Intersection intersection;

			sample = samples.back();
			samples.pop_back();
			camera.genSampleRay(sample, &ray, aspectRatio);

			while(numBounces <= maxBounces && world.isIntersecting(ray, &intersection))
			{
				const Model* hitModel = intersection.getHitPrimitive()->getParentModel();
				const Material* hitMaterial = hitModel->getMaterial();

				Vector3f L;
				Vector3f N(intersection.getHitNormal());
				Vector3f V(ray.getDirection().mul(-1.0f));

				hitMaterial->getSurfaceIntegrand()->genImportanceRandomV(intersection, V, &L);

				if(hitMaterial->getSurfaceIntegrand()->isEmissive())
				{
					Vector3f radiance;
					hitMaterial->getSurfaceIntegrand()->evaluateEmittedRadiance(intersection, L, V, &radiance);

					ray.addLiRadiance(radiance);
					ray.calcWeightedLiRadiance(&radiance);
					accuRadiance.addLocal(radiance);

					break;
				}

				Vector3f liWeight;
				Vector3f pdf;
				hitMaterial->getSurfaceIntegrand()->evaluateLiWeight(intersection, L, V, &liWeight);
				hitMaterial->getSurfaceIntegrand()->evaluateImportanceRandomVPDF(intersection, L, V, &pdf);

				ray.accumulateLiWeight(liWeight.div(pdf));

				Vector3f nextRayOrigin(intersection.getHitPosition().add(N.mul(0.0001f)));
				Vector3f nextRayDirection(L);
				ray.setOrigin(nextRayOrigin);
				ray.setDirection(nextRayDirection);

				numBounces++;
			}// end while

			Vector3f pixel;
			uint32 x = static_cast<uint32>((sample.m_cameraX + 1.0f) / 2.0f * out_frame->getWidthPx());
			uint32 y = static_cast<uint32>((sample.m_cameraY + 1.0f) / 2.0f * out_frame->getHeightPx());

			//std::cout << x << ", " << y << std::endl;

			out_frame->getPixel(x, y, &pixel);
			pixel.addLocal(accuRadiance.div(static_cast<float32>(spp)));
			out_frame->setPixel(x, y, pixel.x, pixel.y, pixel.z);
		}// end while

		std::cout << "SPP: " << ++numSpp << std::endl;
	}
}

}// end namespace ph