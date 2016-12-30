#include "Core/SurfaceBehavior/TranslucentMicrofacet.h"
#include "Image/ConstantTexture.h"
#include "Core/Ray.h"
#include "Math/Vector3f.h"
#include "Math/random_number.h"
#include "Math/constant.h"
#include "Core/Intersection.h"
#include "Core/SurfaceBehavior/random_sample.h"
#include "Core/SurfaceBehavior/Microfacet.h"
#include "Core/SurfaceBehavior/SurfaceSample.h"

#include <memory>
#include <iostream>

namespace ph
{

TranslucentMicrofacet::TranslucentMicrofacet() :
	m_F0       (std::make_shared<ConstantTexture>(Vector3f(0.04f, 0.04f, 0.04f))),
	m_IOR      (std::make_shared<ConstantTexture>(Vector3f(1.0f, 1.0f, 1.0f))),
	m_roughness(std::make_shared<ConstantTexture>(Vector3f(0.5f, 0.5f, 0.5f)))
{

}

TranslucentMicrofacet::~TranslucentMicrofacet() = default;

void TranslucentMicrofacet::genImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const
{
	// HACK: copy-and-paste comment
	// Cook-Torrance microfacet specular BRDF is D(H)*F(V, H)*G(L, V, H) / (4*NoL*NoV).
	// The importance sampling strategy is to generate a microfacet normal (H) which follows D(H)'s distribution, and
	// generate L by reflecting/refracting -V using H.
	// The PDF for this sampling scheme is D(H)*NoH / (4*HoL). The reason that (4*HoL) exists is because there's a 
	// jacobian involved (from H's probability space to L's).

	Vector3f sampledRoughness;
	m_roughness->sample(intersection.getHitUVW(), &sampledRoughness);
	const float32 roughness = sampledRoughness.x;

	Vector3f sampledF0;
	m_F0->sample(intersection.getHitUVW(), &sampledF0);

	const Vector3f V = ray.getDirection().mul(-1.0f);
	const Vector3f& N = intersection.getHitSmoothNormal();
	Vector3f H;

	genUnitHemisphereGgxTrowbridgeReitzNdfSample(genRandomFloat32_0_1_uniform(), genRandomFloat32_0_1_uniform(), roughness, &H);
	Vector3f u;
	Vector3f v(N);
	Vector3f w;
	v.calcOrthBasisAsYaxis(&u, &w);
	H = u.mulLocal(H.x).addLocal(v.mulLocal(H.y)).addLocal(w.mulLocal(H.z));
	H.normalizeLocal();

	const float32 NoV = N.dot(V);
	const float32 HoV = H.dot(V);
	const float32 NoH = N.dot(H);

	Vector3f F;
	Microfacet::fresnelSchlickApproximated(abs(HoV), sampledF0, &F);

	// use Fresnel term to select which path to take and calculate L

	const float32 dart = genRandomFloat32_0_1_uniform();
	const float32 reflectProb = F.avg();

	// reflect path
	if(dart < reflectProb)
	{
		// calculate reflected L
		out_sample->m_direction = ray.getDirection().reflect(H).normalizeLocal();

		// account for probability
		F.divLocal(reflectProb);

		// this path reflects light
		out_sample->m_type = ESurfaceSampleType::REFLECTION;
	}
	// refract path
	else
	{
		float32 signHoV = HoV < 0.0f ? -1.0f : 1.0f;
		Vector3f ior;
		m_IOR->sample(intersection.getHitUVW(), &ior);

		// assume the outside medium has an IOR of 1.0 (which is true in most cases)
		const float32 iorRatio = signHoV < 0.0f ? ior.x : 1.0f / ior.x;
		const float32 sqrValue = 1.0f - iorRatio*iorRatio*(1.0f - HoV * HoV);

		// TIR (total internal reflection)
		if(sqrValue <= 0.0f)
		{
			// calculate reflected L
			out_sample->m_direction = V.mul(-1.0f).reflectLocal(H).normalizeLocal();

			// this path reflects light
			out_sample->m_type = ESurfaceSampleType::REFLECTION;

			// account for probability
			F = F.complement().divLocal(1.0f - reflectProb);
		}
		// refraction
		else
		{
			// calculate refracted L
			const float32 Hfactor = iorRatio * HoV - signHoV * sqrt(sqrValue);
			const float32 Vfactor = -iorRatio;
			out_sample->m_direction = H.mul(Hfactor).addLocal(V.mul(Vfactor)).normalizeLocal();

			// this path refracts light
			out_sample->m_type = ESurfaceSampleType::TRANSMISSION;

			// account for probability
			F = F.complement().divLocal(1.0f - reflectProb);
		}
	}

	const Vector3f& L = out_sample->m_direction;

	const float32 NoL = N.dot(L);
	const float32 HoL = H.dot(L);

	// sidedness agreement between real geometry and shading (phong-interpolated) normal
	if(NoV * intersection.getHitGeoNormal().dot(V) <= 0.0f || NoL * intersection.getHitGeoNormal().dot(L) <= 0.0f)
	{
		out_sample->m_LiWeight.set(0, 0, 0);
		out_sample->m_direction.set(ray.getDirection().reflect(H).normalizeLocal());
		out_sample->m_type = ESurfaceSampleType::REFLECTION;
		out_sample->m_emittedRadiance.set(0, 0, 0);
		//std::cerr << "detected!";
		return;
	}

	const float32 G = Microfacet::geometryShadowingGgxSmith(NoV, NoL, HoV, HoL, roughness);

	// notice that the (N dot L) term canceled out with the lambertian term
	const float32 dotTerms = abs(HoL / (NoV * NoH));
	out_sample->m_LiWeight.set(F.mul(G * dotTerms));
}

void TranslucentMicrofacet::evaluate(const Intersection& intersection, const Vector3f& wi, const Vector3f& wo, Vector3f* const out_value) const
{
	std::cerr << "warning: TranslucentMicrofacet::evaluate() not implemented" << std::endl;
}

}// end namespace ph