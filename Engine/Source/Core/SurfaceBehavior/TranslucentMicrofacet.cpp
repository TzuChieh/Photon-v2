#include "Core/SurfaceBehavior/TranslucentMicrofacet.h"
#include "Actor/Texture/ConstantTexture.h"
#include "Core/Ray.h"
#include "Math/Vector3f.h"
#include "Math/random_number.h"
#include "Math/constant.h"
#include "Core/Intersection.h"
#include "Core/SurfaceBehavior/random_sample.h"
#include "Core/SurfaceBehavior/Microfacet.h"
#include "Core/Sample/SurfaceSample.h"

#include <memory>
#include <iostream>

namespace ph
{

TranslucentMicrofacet::TranslucentMicrofacet() :
	m_F0   (std::make_shared<ConstantTexture>(Vector3f(0.04f, 0.04f, 0.04f))),
	m_IOR  (std::make_shared<ConstantTexture>(Vector3f(1.0f, 1.0f, 1.0f))),
	m_alpha(std::make_shared<ConstantTexture>(Vector3f(0.5f, 0.5f, 0.5f)))
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

	Vector3f sampledAlpha;
	m_alpha->sample(sample.X->getHitUVW(), &sampledAlpha);
	const float32 alpha = sampledAlpha.x;

	Vector3f sampledF0;
	m_F0->sample(sample.X->getHitUVW(), &sampledF0);

	const Vector3f V = sample.V;
	const Vector3f& N = sample.X->getHitSmoothNormal();
	Vector3f H;

	genUnitHemisphereGgxTrowbridgeReitzNdfSample(genRandomFloat32_0_1_uniform(), genRandomFloat32_0_1_uniform(), alpha, &H);
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

	const float32 G = Microfacet::geometryShadowingGgxSmith(NoV, NoL, HoV, HoL, alpha);

	// notice that the (N dot L) term canceled out with the lambertian term
	const float32 dotTerms = abs(HoL / (NoV * NoH));
	sample.liWeight.set(F.mul(G * dotTerms));
}

float32 TranslucentMicrofacet::calcImportanceSamplePdfW(const SurfaceSample& sample) const
{
	const Vector3f& N = sample.X->getHitSmoothNormal();
	const float32 NoL = N.dot(sample.L);

	Vector3f sampledAlpha;
	m_alpha->sample(sample.X->getHitUVW(), &sampledAlpha);
	const float32 alpha = sampledAlpha.x;

	Vector3f sampledF0;
	m_F0->sample(sample.X->getHitUVW(), &sampledF0);

	switch(sample.type)
	{
	case ESurfaceSampleType::REFLECTION:
	{
		// H is on the hemisphere of N
		Vector3f H = sample.L.add(sample.V).normalizeLocal();
		if(NoL < 0.0f)
		{
			H.mulLocal(-1.0f);
		}

		const float32 NoH = N.dot(H);
		const float32 HoL = H.dot(sample.L);
		const float32 HoV = H.dot(sample.V);
		const float32 D = Microfacet::normalDistributionGgxTrowbridgeReitz(NoH, alpha);

		Vector3f F;
		Microfacet::fresnelSchlickApproximated(abs(HoV), sampledF0, &F);
		float32 reflectProb = F.avg();

		const float32 signHoV = HoV < 0.0f ? -1.0f : 1.0f;
		Vector3f ior;
		m_IOR->sample(sample.X->getHitUVW(), &ior);

		// assume the outside medium has an IOR of 1.0 (which is true in most cases)
		const float32 iorRatio = signHoV < 0.0f ? ior.x : 1.0f / ior.x;
		const float32 sqrValue = 1.0f - iorRatio*iorRatio*(1.0f - HoV * HoV);

		// TIR (total internal reflection)
		if(sqrValue <= 0.0f)
		{
			reflectProb = 1.0f;
		}

		return std::abs(D * NoH / (4.0f * HoL)) * reflectProb;
		break;
	}

	case ESurfaceSampleType::TRANSMISSION:
	{
		Vector3f sampledIor;
		m_IOR->sample(sample.X->getHitUVW(), &sampledIor);
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
		H = sample.L.mul(iorI).add(sample.V.mul(iorO)).mulLocal(-1.0f).normalizeLocal();

		const float32 HoV = H.dot(sample.V);
		const float32 NoH = N.dot(H);
		const float32 HoL = H.dot(sample.L);

		const float32 D = Microfacet::normalDistributionGgxTrowbridgeReitz(NoH, alpha);

		Vector3f F;
		Microfacet::fresnelSchlickApproximated(abs(HoV), sampledF0, &F);
		const float32 reflectProb = 1.0f - F.avg();

		const float32 iorTerm = iorI*HoL + iorO*HoV;
		const float32 multiplier = iorI * iorI * HoV / (iorTerm * iorTerm);

		return std::abs(D * NoH * multiplier) * reflectProb;
		break;
	}

	default:
		std::cerr << "warning: at TranslucentMicrofacet::calcImportanceSamplePdfW(), invalid sample type detected" << std::endl;
		return 0.0f;
		break;
	}
}

void TranslucentMicrofacet::evaluate(SurfaceSample& sample) const
{
	const Vector3f& N = sample.X->getHitSmoothNormal();

	const float32 NoL = N.dot(sample.L);
	const float32 NoV = N.dot(sample.V);

	Vector3f sampledAlpha;
	m_alpha->sample(sample.X->getHitUVW(), &sampledAlpha);
	const float32 alpha = sampledAlpha.x;
	Vector3f sampledF0;
	m_F0->sample(sample.X->getHitUVW(), &sampledF0);

	// reflection
	if(NoL * NoV >= 0.0f)
	{
		// DEBUG
		/*sample.liWeight.set(0, 0, 0);
		return;*/

		// H is on the hemisphere of N
		Vector3f H = sample.L.add(sample.V).normalizeLocal();
		if(N.dot(H) < 0.0f)
		{
			H.mulLocal(-1.0f);
		}

		const float32 HoV = H.dot(sample.V);
		const float32 NoH = N.dot(H);
		const float32 HoL = H.dot(sample.L);

		Vector3f F;
		Microfacet::fresnelSchlickApproximated(std::abs(HoV), sampledF0, &F);

		const float32 signHoV = HoV < 0.0f ? -1.0f : 1.0f;
		Vector3f ior;
		m_IOR->sample(sample.X->getHitUVW(), &ior);

		// assume the outside medium has an IOR of 1.0 (which is true in most cases)
		const float32 iorRatio = signHoV < 0.0f ? ior.x : 1.0f / ior.x;
		const float32 sqrValue = 1.0f - iorRatio*iorRatio*(1.0f - HoV * HoV);

		// TIR (total internal reflection)
		if(sqrValue <= 0.0f)
		{
			F.set(1.0f, 1.0f, 1.0f);
		}

		const float32 D = Microfacet::normalDistributionGgxTrowbridgeReitz(NoH, alpha);
		const float32 G = Microfacet::geometryShadowingGgxSmith(NoV, NoL, HoV, HoL, alpha);

		// notice that the abs(N dot L) term canceled out with the lambertian term
		sample.liWeight = F.mul(D * G / (4.0f * std::abs(NoV)));
		sample.type = ESurfaceSampleType::REFLECTION;
	}
	// refraction
	else
	{
		// DEBUG
		/*sample.liWeight.set(0, 0, 0);
		return;*/

		Vector3f sampledIor;
		m_IOR->sample(sample.X->getHitUVW(), &sampledIor);
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
		H = sample.L.mul(iorI).add(sample.V.mul(iorO)).mulLocal(-1.0f).normalizeLocal();

		const float32 HoV = H.dot(sample.V);
		const float32 NoH = N.dot(H);
		const float32 HoL = H.dot(sample.L);

		//const float32 gNoV = sample.X->getHitGeoNormal().dot(sample.V);
		//const float32 gNoL = sample.X->getHitGeoNormal().dot(sample.L);
		//const float32 gNoH = sample.X->getHitGeoNormal().dot(H);
		////if(HoV * HoL >= 0.0f || NoH <= 0.0f || NoV * HoV <= 0.0f || NoL * HoL <= 0.0f || gNoV * gNoL >= 0.0f)
		//if(HoV * HoL >= 0.0f || gNoH <= 0.0f || gNoV * HoV <= 0.0f || gNoL * HoL <= 0.0f || gNoV * gNoL >= 0.0f)
		//{
		//	sample.liWeight.set(0, 0, 0);
		//	return;
		//}
		
		Vector3f F;
		Microfacet::fresnelSchlickApproximated(std::abs(HoV), sampledF0, &F);
		const float32 D = Microfacet::normalDistributionGgxTrowbridgeReitz(NoH, alpha);
		const float32 G = Microfacet::geometryShadowingGgxSmith(NoV, NoL, HoV, HoL, alpha);

		// notice that the abs(N dot L) term canceled out with the lambertian term
		const float32 dotTerm = std::abs(HoL * HoV / NoV);
		const float32 iorTerm = iorI / (iorI * HoL + iorO * HoV);
		sample.liWeight = F.complement().mul(D * G * dotTerm * (iorTerm * iorTerm));
		sample.type = ESurfaceSampleType::TRANSMISSION;
	}
}

}// end namespace ph