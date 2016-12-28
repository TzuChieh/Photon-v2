#include "Core/SurfaceBehavior/LambertianDiffuse.h"
#include "Core/Ray.h"
#include "Math/Vector3f.h"
#include "Math/random_number.h"
#include "Math/constant.h"
#include "Actor/Material/MatteOpaque.h"
#include "Core/Intersection.h"
#include "Core/SurfaceBehavior/random_sample.h"
#include "Core/SurfaceBehavior/SurfaceSample.h"

#include <cmath>

namespace ph
{

LambertianDiffuse::LambertianDiffuse() :
	m_albedo(std::make_shared<ConstantTexture>(Vector3f(0.5f, 0.5f, 0.5f)))
{

}

LambertianDiffuse::~LambertianDiffuse() = default;

void LambertianDiffuse::genImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const
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
	Vector3f v(intersection.getHitSmoothNormal());
	Vector3f w;
	v.calcOrthBasisAsYaxis(&u, &w);
	L = u.mulLocal(L.x).addLocal(v.mulLocal(L.y)).addLocal(w.mulLocal(L.z));
	L.normalizeLocal();

	// this model reflects light
	out_sample->m_type = ESurfaceSampleType::REFLECTION;
}

void LambertianDiffuse::setAlbedo(const std::shared_ptr<Texture>& albedo)
{
	m_albedo = albedo;
}

}// end namespace ph