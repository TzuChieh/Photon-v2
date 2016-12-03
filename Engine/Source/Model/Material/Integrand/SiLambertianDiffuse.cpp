#include "Model/Material/Integrand/SiLambertianDiffuse.h"
#include "Core/Ray.h"
#include "Math/Vector3f.h"
#include "Math/random_number.h"
#include "Math/constant.h"
#include "Model/Material/MatteOpaque.h"
#include "Core/Intersection.h"
#include "Model/Material/Integrand/random_sample.h"

#include <cmath>

namespace ph
{

SiLambertianDiffuse::SiLambertianDiffuse() :
	m_albedo(std::make_shared<ConstantTexture>(Vector3f(0.5f, 0.5f, 0.5f)))
{

}

SiLambertianDiffuse::~SiLambertianDiffuse() = default;

void SiLambertianDiffuse::genUniformRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const
{
	const float32 rand1 = genRandomFloat32_0_1_uniform();
	const float32 rand2 = genRandomFloat32_0_1_uniform();

	const float32 phi     = 2.0f * PI_FLOAT32 * rand1;
	const float32 yValue  = rand2;
	const float32 yRadius = sqrt(1.0f - yValue * yValue);

	out_V->x = cos(phi) * yRadius;
	out_V->y = yValue;
	out_V->z = sin(phi) * yRadius;

	Vector3f u;
	Vector3f v(intersection.getHitNormal());
	Vector3f w;

	v.calcOrthBasisAsYaxis(&u, &w);

	*out_V = u.mulLocal(out_V->x).
	addLocal(v.mulLocal(out_V->y)).
	addLocal(w.mulLocal(out_V->z));

	out_V->normalizeLocal();
}

void SiLambertianDiffuse::genImportanceRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const
{
	const float32 rand1 = genRandomFloat32_0_1_uniform();
	const float32 rand2 = genRandomFloat32_0_1_uniform();

	const float32 phi = 2.0f * PI_FLOAT32 * rand1;
	const float32 yValue = sqrt(rand2);
	const float32 yRadius = sqrt(1.0f - yValue * yValue);

	out_V->x = cos(phi) * yRadius;
	out_V->y = yValue;
	out_V->z = sin(phi) * yRadius;

	Vector3f u;
	Vector3f v(intersection.getHitNormal());
	Vector3f w;

	v.calcOrthBasisAsYaxis(&u, &w);

	*out_V = u.mulLocal(out_V->x).
	addLocal(v.mulLocal(out_V->y)).
	addLocal(w.mulLocal(out_V->z));

	out_V->normalizeLocal();
}

void SiLambertianDiffuse::evaluateUniformRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const
{
	out_PDF->set(1.0f / (2.0f * PI_FLOAT32));
}

void SiLambertianDiffuse::evaluateImportanceRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const
{
	out_PDF->set(intersection.getHitNormal().dot(L) / PI_FLOAT32);
}

void SiLambertianDiffuse::evaluateLiWeight(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_LiWeight) const
{
	Vector3f brdf;
	m_albedo->sample(intersection.getHitUVW(), &brdf);
	brdf.mulLocal(1.0f / PI_FLOAT32);

	out_LiWeight->set(brdf.mulLocal(intersection.getHitNormal().dot(L)));
}

void SiLambertianDiffuse::evaluateImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const
{
	// Lambertian diffuse model's BRDF is simply albedo/pi.
	// The importance sampling strategy is to use the cosine term in the rendering equation, 
	// generating a cos(theta) weighted L corresponding to N, which PDF is cos(theta)/pi.
	// Thus, BRDF_lambertian*cos(theta)/PDF = albedo = Li's weight.

	Vector3f albedo;
	m_albedo->sample(intersection.getHitUVW(), &albedo);
	out_sample->m_LiWeight.set(albedo);

	// generate and transform L to N's space

	Vector3f& L = out_sample->m_direction;
	genUnitHemisphereCosineThetaWeightedSample(genRandomFloat32_0_1_uniform(), genRandomFloat32_0_1_uniform(), &L);
	Vector3f u;
	Vector3f v(intersection.getHitNormal());
	Vector3f w;
	v.calcOrthBasisAsYaxis(&u, &w);
	L = u.mulLocal(L.x).addLocal(v.mulLocal(L.y)).addLocal(w.mulLocal(L.z));
	L.normalizeLocal();

	// this model reflects light
	out_sample->m_type = ESurfaceSampleType::REFLECTION;
}

void SiLambertianDiffuse::setAlbedo(const std::shared_ptr<Texture>& albedo)
{
	m_albedo = albedo;
}

}// end namespace ph