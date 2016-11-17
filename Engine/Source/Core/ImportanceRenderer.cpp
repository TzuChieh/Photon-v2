#include "Core/ImportanceRenderer.h"
#include "Common/primitive_type.h"
#include "Image/Film.h"
#include "World/World.h"
#include "Camera/Camera.h"
#include "Core/Ray.h"
#include "Core/Intersection.h"
#include "Model/Material/Material.h"
#include "Model/Material/SurfaceIntegrand.h"
#include "Math/constant.h"
#include "Core/SampleGenerator.h"
#include "Core/Sample.h"
#include "Math/random_number.h"
#include "Math/Color.h"
#include "Math/Math.h"

#include <cmath>
#include <iostream>
#include <vector>

namespace ph
{

ImportanceRenderer::~ImportanceRenderer() = default;

void ImportanceRenderer::render(const World& world, const Camera& camera) const
{
	Film* film = camera.getFilm();

	const uint32 widthPx = film->getWidthPx();
	const uint32 heightPx = film->getHeightPx();

	const float32 aspectRatio = static_cast<float32>(widthPx) / static_cast<float32>(heightPx);

	const uint32 maxBounces = 10000;
	std::vector<Sample> samples;

	int32 numSpp = 0;

	while(m_sampleGenerator->hasMoreSamples())
	{
		samples.clear();
		m_sampleGenerator->requestMoreSamples(*film, &samples);

		Sample sample;
		while(!samples.empty())
		{
			uint32 numBounces = 0;
			Vector3f accuRadiance(0, 0, 0);
			Vector3f accuLiWeight(1, 1, 1);
			Ray ray;
			Intersection intersection;

			sample = samples.back();
			samples.pop_back();
			camera.genSampleRay(sample, &ray, aspectRatio);

			while(numBounces <= maxBounces && world.isIntersecting(ray, &intersection))
			{
				const Model* hitModel = intersection.getHitPrimitive()->getParentModel();
				const Material* hitMaterial = hitModel->getMaterial();

				const Vector3f N(intersection.getHitNormal());
				const Vector3f V(ray.getDirection().mul(-1.0f));
				Vector3f L;

				hitMaterial->getSurfaceIntegrand()->genImportanceRandomV(intersection, V, &L);

				if(hitMaterial->getSurfaceIntegrand()->isEmissive())
				{
					Vector3f radianceLi;
					hitMaterial->getSurfaceIntegrand()->evaluateEmittedRadiance(intersection, L, V, &radianceLi);

					// avoid excessive, negative weight and possible NaNs
					accuLiWeight.clampLocal(0.0f, 1000.0f);

					accuRadiance.addLocal(radianceLi.mul(accuLiWeight));

					/*if(radianceLi.x != 1.0f || radianceLi.y != 1.0f || radianceLi.z != 1.0f) 
						std::cout << radianceLi.toStringFormal() << std::endl;*/

					break;
				}

				Vector3f liWeight;
				Vector3f pdf;
				hitMaterial->getSurfaceIntegrand()->evaluateLiWeight(intersection, L, V, &liWeight);
				hitMaterial->getSurfaceIntegrand()->evaluateImportanceRandomVPDF(intersection, L, V, &pdf);

				liWeight.divLocal(pdf);

				//const float32 rrSurviveRate = liWeight.clamp(0.0f, 1.0f).max();
				const float32 rrSurviveRate = Math::clamp(liWeight.avg(), 0.0001f, 1.0f);
				//const float32 rrSurviveRate = Math::clamp(Color::linearRgbLuminance(liWeight), 0.0001f, 1.0f);
				const float32 rrSpin = genRandomFloat32_0_1_uniform();

				// russian roulette >> survive
				if(rrSurviveRate > rrSpin)
				{
					const float32 rrScale = 1.0f / rrSurviveRate;
					liWeight.mulLocal(rrScale);
				}
				// russian roulette >> dead
				else
				{
					break;
				}

				accuLiWeight.mulLocal(liWeight);

				// prepare for next recursion
				const Vector3f nextRayOrigin(intersection.getHitPosition().add(N.mul(0.0001f)));
				const Vector3f nextRayDirection(L);
				ray.setOrigin(nextRayOrigin);
				ray.setDirection(nextRayDirection);
				numBounces++;
			}// end while

			uint32 x = static_cast<uint32>((sample.m_cameraX + 1.0f) / 2.0f * film->getWidthPx());
			uint32 y = static_cast<uint32>((sample.m_cameraY + 1.0f) / 2.0f * film->getHeightPx());
			if(x >= film->getWidthPx()) x = film->getWidthPx() - 1;
			if(y >= film->getHeightPx()) y = film->getHeightPx() - 1;

			film->acculumateRadiance(x, y, accuRadiance);
		}// end while

		std::cout << "SPP: " << ++numSpp << std::endl;
	}
}

}// end namespace ph