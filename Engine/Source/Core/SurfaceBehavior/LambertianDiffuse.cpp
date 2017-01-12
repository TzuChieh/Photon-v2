#include "Core/SurfaceBehavior/LambertianDiffuse.h"
#include "Core/Ray.h"
#include "Math/Vector3f.h"
#include "Math/random_number.h"
#include "Math/constant.h"
#include "Actor/Material/MatteOpaque.h"
#include "Core/Intersection.h"
#include "Core/SurfaceBehavior/random_sample.h"
#include "Core/Sample/SurfaceSample.h"

#include <cmath>

namespace ph
{

LambertianDiffuse::LambertianDiffuse() :
	m_albedo(std::make_shared<ConstantTexture>(Vector3f(0.5f, 0.5f, 0.5f)))
{

}

LambertianDiffuse::~LambertianDiffuse() = default;

void LambertianDiffuse::genImportanceSample(SurfaceSample& sample) const
{
	// Lambertian diffuse model's BRDF is simply albedo/pi.
	// The importance sampling strategy is to use the cosine term in the rendering equation, 
	// generating a cos(theta) weighted L corresponding to N, which PDF is cos(theta)/pi.
	// Thus, BRDF_lambertian*cos(theta)/PDF = albedo = Li's weight.

	Vector3f albedo;
	m_albedo->sample(sample.X->getHitUVW(), &albedo);
	sample.liWeight.set(albedo);

	// generate and transform L to N's space

	Vector3f& L = sample.L;
	genUnitHemisphereCosineThetaWeightedSample(genRandomFloat32_0_1_uniform(), genRandomFloat32_0_1_uniform(), &L);
	Vector3f u;
	Vector3f v(sample.X->getHitSmoothNormal());
	Vector3f w;
	v.calcOrthBasisAsYaxis(&u, &w);
	L = u.mulLocal(L.x).addLocal(v.mulLocal(L.y)).addLocal(w.mulLocal(L.z));
	L.normalizeLocal();

	if(sample.V.dot(sample.X->getHitSmoothNormal()) < 0.0f)
	{
		L.mulLocal(-1.0f);
	}

	// this model reflects light
	sample.type = ESurfaceSampleType::REFLECTION;
}

float32 LambertianDiffuse::calcImportanceSamplePdfW(const SurfaceSample& sample) const
{
	return sample.L.dot(sample.X->getHitSmoothNormal()) * RECI_PI_FLOAT32;
}

void LambertianDiffuse::setAlbedo(const std::shared_ptr<Texture>& albedo)
{
	m_albedo = albedo;
}

void LambertianDiffuse::evaluate(SurfaceSample& sample) const
{
	const float32 NoL = sample.X->getHitSmoothNormal().dot(sample.L);
	const float32 NoV = sample.X->getHitSmoothNormal().dot(sample.V);

	// check if L, V lies on different side of the surface
	if(NoL * NoV <= 0.0f)
	{
		sample.liWeight.set(0, 0, 0);
		return;
	}

	Vector3f albedo;
	m_albedo->sample(sample.X->getHitUVW(), &albedo);
	sample.liWeight = albedo.divLocal(PI_FLOAT32).mulLocal(std::abs(NoL));
	sample.type = ESurfaceSampleType::REFLECTION;
}

}// end namespace ph