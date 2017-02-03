#include "Core/SurfaceBehavior/LambertianDiffuse.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"
#include "Math/random_number.h"
#include "Math/constant.h"
#include "Actor/Material/MatteOpaque.h"
#include "Core/Intersection.h"
#include "Core/SurfaceBehavior/random_sample.h"
#include "Core/Sample/SurfaceSample.h"
#include "Math/Math.h"

#include <cmath>

namespace ph
{

LambertianDiffuse::LambertianDiffuse() :
	m_albedo(std::make_shared<ConstantTexture>(Vector3R(0.5_r, 0.5_r, 0.5_r)))
{

}

LambertianDiffuse::~LambertianDiffuse() = default;

void LambertianDiffuse::genImportanceSample(SurfaceSample& sample) const
{
	// Lambertian diffuse model's BRDF is simply albedo/pi.
	// The importance sampling strategy is to use the cosine term in the rendering equation, 
	// generating a cos(theta) weighted L corresponding to N, which PDF is cos(theta)/pi.
	// Thus, BRDF_lambertian*cos(theta)/PDF = albedo = Li's weight.

	Vector3R albedo;
	m_albedo->sample(sample.X->getHitUVW(), &albedo);
	sample.liWeight.set(albedo);

	// generate and transform L to N's space

	Vector3R& L = sample.L;
	genUnitHemisphereCosineThetaWeightedSample(genRandomReal_0_1_uniform(), genRandomReal_0_1_uniform(), &L);
	Vector3R u;
	Vector3R v(sample.X->getHitSmoothNormal());
	Vector3R w;
	Math::formOrthonormalBasis(v, &u, &w);
	L = u.mulLocal(L.x).addLocal(v.mulLocal(L.y)).addLocal(w.mulLocal(L.z));
	L.normalizeLocal();

	if(sample.V.dot(sample.X->getHitSmoothNormal()) < 0.0_r)
	{
		L.mulLocal(-1.0_r);
	}

	// this model reflects light
	sample.type = ESurfaceSampleType::REFLECTION;
}

real LambertianDiffuse::calcImportanceSamplePdfW(const SurfaceSample& sample) const
{
	return sample.L.dot(sample.X->getHitSmoothNormal()) * RECI_PI_REAL;
}

void LambertianDiffuse::setAlbedo(const std::shared_ptr<Texture>& albedo)
{
	m_albedo = albedo;
}

void LambertianDiffuse::evaluate(SurfaceSample& sample) const
{
	const real NoL = sample.X->getHitSmoothNormal().dot(sample.L);
	const real NoV = sample.X->getHitSmoothNormal().dot(sample.V);

	// check if L, V lies on different side of the surface
	if(NoL * NoV <= 0.0_r)
	{
		sample.liWeight.set(0, 0, 0);
		return;
	}

	Vector3R albedo;
	m_albedo->sample(sample.X->getHitUVW(), &albedo);
	sample.liWeight = albedo.divLocal(PI_REAL).mulLocal(std::abs(NoL));
	sample.type = ESurfaceSampleType::REFLECTION;
}

}// end namespace ph