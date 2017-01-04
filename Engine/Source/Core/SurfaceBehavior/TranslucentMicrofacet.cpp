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

void TranslucentMicrofacet::genImportanceSample(SurfaceSample& sample) const
{
	// Cook-Torrance microfacet specular BRDF for translucent surface is:
	// |HoL||HoV|/(|NoL||NoV|)*(iorO^2)*(D(H)*F(V, H)*G(L, V, H)) / (iorI*HoL + iorO*HoV)^2.
	// The importance sampling strategy is to generate a microfacet normal (H) which follows D(H)'s distribution, and
	// generate L by reflecting/refracting -V using H.
	// The PDF for this sampling scheme is (D(H)*NoH) * (iorO^2 * |HoV| / ((iorI*HoL + iorO*HoV)^2)).
	// The reason that the latter multiplier in the PDF exists is because there's a jacobian involved (from H's probability space to L's).

	Vector3f sampledRoughness;
	m_roughness->sample(sample.X->getHitUVW(), &sampledRoughness);
	const float32 roughness = sampledRoughness.x;

	Vector3f sampledF0;
	m_F0->sample(sample.X->getHitUVW(), &sampledF0);

	const Vector3f V = sample.V;
	const Vector3f& N = sample.X->getHitSmoothNormal();
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
		sample.L = sample.V.mul(-1.0f).reflect(H).normalizeLocal();

		// account for probability
		F.divLocal(reflectProb);

		// this path reflects light
		sample.type = ESurfaceSampleType::REFLECTION;
	}
	// refract path
	else
	{
		float32 signHoV = HoV < 0.0f ? -1.0f : 1.0f;
		Vector3f ior;
		m_IOR->sample(sample.X->getHitUVW(), &ior);

		// assume the outside medium has an IOR of 1.0 (which is true in most cases)
		const float32 iorRatio = signHoV < 0.0f ? ior.x : 1.0f / ior.x;
		const float32 sqrValue = 1.0f - iorRatio*iorRatio*(1.0f - HoV * HoV);

		// TIR (total internal reflection)
		if(sqrValue <= 0.0f)
		{
			// calculate reflected L
			sample.L = V.mul(-1.0f).reflectLocal(H).normalizeLocal();

			// this path reflects light
			sample.type = ESurfaceSampleType::REFLECTION;

			// account for probability
			F = F.complement().divLocal(1.0f - reflectProb);
		}
		// refraction
		else
		{
			// calculate refracted L
			const float32 Hfactor = iorRatio * HoV - signHoV * sqrt(sqrValue);
			const float32 Vfactor = -iorRatio;
			sample.L = H.mul(Hfactor).addLocal(V.mul(Vfactor)).normalizeLocal();

			// this path refracts light
			sample.type = ESurfaceSampleType::TRANSMISSION;

			// account for probability
			F = F.complement().divLocal(1.0f - reflectProb);
		}
	}

	const Vector3f& L = sample.L;

	const float32 NoL = N.dot(L);
	const float32 HoL = H.dot(L);

	// sidedness agreement between real geometry and shading (phong-interpolated) normal
	if(NoV * sample.X->getHitGeoNormal().dot(V) <= 0.0f || NoL * sample.X->getHitGeoNormal().dot(L) <= 0.0f)
	{
		sample.liWeight.set(0, 0, 0);
		return;
	}

	const float32 G = Microfacet::geometryShadowingGgxSmith(NoV, NoL, HoV, HoL, roughness);

	// notice that the (N dot L) term canceled out with the lambertian term
	const float32 dotTerms = abs(HoL / (NoV * NoH));
	sample.liWeight.set(F.mul(G * dotTerms));
}

void TranslucentMicrofacet::evaluate(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_value) const
{
	const Vector3f& N = intersection.getHitSmoothNormal();

	const float32 NoL = N.dot(L);
	const float32 NoV = N.dot(V);

	// sidedness agreement between real geometry and shading (phong-interpolated) normal
	if(NoL * intersection.getHitGeoNormal().dot(L) <= 0.0f || NoV * intersection.getHitGeoNormal().dot(V) <= 0.0f)
	{
		out_value->set(0, 0, 0);
		return;
	}

	Vector3f sampledRoughness;
	m_roughness->sample(intersection.getHitUVW(), &sampledRoughness);
	const float32 roughness = sampledRoughness.x;
	Vector3f sampledF0;
	m_F0->sample(intersection.getHitUVW(), &sampledF0);

	// reflection
	if(NoL * NoV >= 0.0f)
	{
		// H is on the hemisphere of N
		Vector3f H = L.add(V).normalizeLocal();
		if(NoL < 0.0f)
		{
			H.mulLocal(-1.0f);
		}

		const float32 HoV = H.dot(V);
		const float32 NoH = N.dot(H);
		const float32 HoL = H.dot(L);

		Vector3f F;
		Microfacet::fresnelSchlickApproximated(std::abs(HoV), sampledF0, &F);
		const float32 D = Microfacet::normalDistributionGgxTrowbridgeReitz(NoH, roughness);
		const float32 G = Microfacet::geometryShadowingGgxSmith(NoV, NoL, HoV, HoL, roughness);

		// notice that the abs(N dot L) term canceled out with the lambertian term
		*out_value = F.mul(D * G / (4.0f * std::abs(NoV)));
	}
	// refraction
	else
	{
		Vector3f sampledIor;
		m_IOR->sample(intersection.getHitUVW(), &sampledIor);
		float32 iorI;
		float32 iorO;

		// H is on the hemisphere of N
		Vector3f H;
		if(NoL < 0.0f)
		{
			iorI = sampledIor.x;
			iorO = 1.0f;
		}
		else
		{
			iorI = 1.0f;
			iorO = sampledIor.x;
		}
		H = L.mul(iorI).add(V.mul(iorO)).mulLocal(-1.0f).normalizeLocal();

		const float32 HoV = H.dot(V);
		const float32 NoH = N.dot(H);
		const float32 HoL = H.dot(L);

		Vector3f F;
		Microfacet::fresnelSchlickApproximated(std::abs(HoV), sampledF0, &F);
		const float32 D = Microfacet::normalDistributionGgxTrowbridgeReitz(NoH, roughness);
		const float32 G = Microfacet::geometryShadowingGgxSmith(NoV, NoL, HoV, HoL, roughness);

		// notice that the abs(N dot L) term canceled out with the lambertian term
		const float32 dotTerm = std::abs(HoL * HoV / NoV);
		const float32 iorTerm = iorO / (iorI * HoL + iorO * HoV);
		*out_value = F.complement().mul(D * G * dotTerm / (iorTerm * iorTerm));
	}
}

}// end namespace ph