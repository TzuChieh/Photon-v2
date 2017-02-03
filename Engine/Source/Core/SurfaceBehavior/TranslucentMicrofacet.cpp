#include "Core/SurfaceBehavior/TranslucentMicrofacet.h"
#include "Actor/Texture/ConstantTexture.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"
#include "Math/random_number.h"
#include "Math/constant.h"
#include "Core/Intersection.h"
#include "Core/SurfaceBehavior/random_sample.h"
#include "Core/SurfaceBehavior/Microfacet.h"
#include "Core/Sample/SurfaceSample.h"
#include "Math/Math.h"

#include <memory>
#include <iostream>

namespace ph
{

TranslucentMicrofacet::TranslucentMicrofacet() :
	m_F0   (std::make_shared<ConstantTexture>(Vector3R(0.04_r, 0.04_r, 0.04_r))),
	m_IOR  (std::make_shared<ConstantTexture>(Vector3R(1.0_r,  1.0_r,  1.0_r))),
	m_alpha(std::make_shared<ConstantTexture>(Vector3R(0.5_r,  0.5_r,  0.5_r)))
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

	Vector3R sampledAlpha;
	m_alpha->sample(sample.X->getHitUVW(), &sampledAlpha);
	const real alpha = sampledAlpha.x;

	Vector3R sampledF0;
	m_F0->sample(sample.X->getHitUVW(), &sampledF0);

	const Vector3R V = sample.V;
	const Vector3R& N = sample.X->getHitSmoothNormal();
	Vector3R H;

	genUnitHemisphereGgxTrowbridgeReitzNdfSample(genRandomReal_0_1_uniform(), genRandomReal_0_1_uniform(), alpha, &H);
	Vector3R u;
	Vector3R v(N);
	Vector3R w;
	Math::formOrthonormalBasis(v, &u, &w);
	H = u.mulLocal(H.x).addLocal(v.mulLocal(H.y)).addLocal(w.mulLocal(H.z));
	H.normalizeLocal();

	const real NoV = N.dot(V);
	const real HoV = H.dot(V);
	const real NoH = N.dot(H);

	Vector3R F;
	Microfacet::fresnelSchlickApproximated(abs(HoV), sampledF0, &F);

	// use Fresnel term to select which path to take and calculate L

	const real dart = genRandomReal_0_1_uniform();
	const real reflectProb = F.avg();

	// reflect path
	if(dart < reflectProb)
	{
		// calculate reflected L
		sample.L = sample.V.mul(-1.0_r).reflect(H).normalizeLocal();

		// account for probability
		F.divLocal(reflectProb);

		// this path reflects light
		sample.type = ESurfaceSampleType::REFLECTION;
	}
	// refract path
	else
	{
		real signHoV = HoV < 0.0_r ? -1.0_r : 1.0_r;
		Vector3R ior;
		m_IOR->sample(sample.X->getHitUVW(), &ior);

		// assume the outside medium has an IOR of 1.0 (which is true in most cases)
		const real iorRatio = signHoV < 0.0_r ? ior.x : 1.0_r / ior.x;
		const real sqrValue = 1.0_r - iorRatio*iorRatio*(1.0_r - HoV * HoV);

		// TIR (total internal reflection)
		if(sqrValue <= 0.0_r)
		{
			// calculate reflected L
			sample.L = V.mul(-1.0_r).reflectLocal(H).normalizeLocal();

			// this path reflects light
			sample.type = ESurfaceSampleType::REFLECTION;

			// account for probability
			F = F.complement().divLocal(1.0_r - reflectProb);
		}
		// refraction
		else
		{
			// calculate refracted L
			const real Hfactor = iorRatio * HoV - signHoV * std::sqrt(sqrValue);
			const real Vfactor = -iorRatio;
			sample.L = H.mul(Hfactor).addLocal(V.mul(Vfactor)).normalizeLocal();

			// this path refracts light
			sample.type = ESurfaceSampleType::TRANSMISSION;

			// account for probability
			F = F.complement().divLocal(1.0_r - reflectProb);
		}
	}

	const Vector3R& L = sample.L;

	const real NoL = N.dot(L);
	const real HoL = H.dot(L);

	const real G = Microfacet::geometryShadowingGgxSmith(NoV, NoL, HoV, HoL, alpha);

	// notice that the (N dot L) term canceled out with the lambertian term
	const real dotTerms = abs(HoL / (NoV * NoH));
	sample.liWeight.set(F.mul(G * dotTerms));
}

real TranslucentMicrofacet::calcImportanceSamplePdfW(const SurfaceSample& sample) const
{
	const Vector3R& N = sample.X->getHitSmoothNormal();
	const real NoL = N.dot(sample.L);

	Vector3R sampledAlpha;
	m_alpha->sample(sample.X->getHitUVW(), &sampledAlpha);
	const real alpha = sampledAlpha.x;

	Vector3R sampledF0;
	m_F0->sample(sample.X->getHitUVW(), &sampledF0);

	switch(sample.type)
	{
	case ESurfaceSampleType::REFLECTION:
	{
		// H is on the hemisphere of N
		Vector3R H = sample.L.add(sample.V).normalizeLocal();
		if(NoL < 0.0_r)
		{
			H.mulLocal(-1.0_r);
		}

		const real NoH = N.dot(H);
		const real HoL = H.dot(sample.L);
		const real HoV = H.dot(sample.V);
		const real D = Microfacet::normalDistributionGgxTrowbridgeReitz(NoH, alpha);

		Vector3R F;
		Microfacet::fresnelSchlickApproximated(abs(HoV), sampledF0, &F);
		real reflectProb = F.avg();

		const real signHoV = HoV < 0.0_r ? -1.0_r : 1.0_r;
		Vector3R ior;
		m_IOR->sample(sample.X->getHitUVW(), &ior);

		// assume the outside medium has an IOR of 1.0 (which is true in most cases)
		const real iorRatio = signHoV < 0.0_r ? ior.x : 1.0_r / ior.x;
		const real sqrValue = 1.0_r - iorRatio*iorRatio*(1.0_r - HoV * HoV);

		// TIR (total internal reflection)
		if(sqrValue <= 0.0_r)
		{
			reflectProb = 1.0_r;
		}

		return std::abs(D * NoH / (4.0_r * HoL)) * reflectProb;
		break;
	}

	case ESurfaceSampleType::TRANSMISSION:
	{
		Vector3R sampledIor;
		m_IOR->sample(sample.X->getHitUVW(), &sampledIor);
		real iorI;
		real iorO;

		// H is on the hemisphere of N
		Vector3R H;
		if(NoL < 0.0_r)
		{
			iorI = sampledIor.x;
			iorO = 1.0_r;
		}
		else
		{
			iorI = 1.0_r;
			iorO = sampledIor.x;
		}
		H = sample.L.mul(iorI).add(sample.V.mul(iorO)).mulLocal(-1.0_r).normalizeLocal();

		const real HoV = H.dot(sample.V);
		const real NoH = N.dot(H);
		const real HoL = H.dot(sample.L);

		const real D = Microfacet::normalDistributionGgxTrowbridgeReitz(NoH, alpha);

		Vector3R F;
		Microfacet::fresnelSchlickApproximated(abs(HoV), sampledF0, &F);
		const real reflectProb = 1.0_r - F.avg();

		const real iorTerm = iorI*HoL + iorO*HoV;
		const real multiplier = iorI * iorI * HoV / (iorTerm * iorTerm);

		return std::abs(D * NoH * multiplier) * reflectProb;
		break;
	}

	default:
		std::cerr << "warning: at TranslucentMicrofacet::calcImportanceSamplePdfW(), invalid sample type detected" << std::endl;
		return 0.0_r;
		break;
	}
}

void TranslucentMicrofacet::evaluate(SurfaceSample& sample) const
{
	const Vector3R& N = sample.X->getHitSmoothNormal();

	const real NoL = N.dot(sample.L);
	const real NoV = N.dot(sample.V);

	Vector3R sampledAlpha;
	m_alpha->sample(sample.X->getHitUVW(), &sampledAlpha);
	const real alpha = sampledAlpha.x;
	Vector3R sampledF0;
	m_F0->sample(sample.X->getHitUVW(), &sampledF0);

	// reflection
	if(NoL * NoV >= 0.0_r)
	{
		// H is on the hemisphere of N
		Vector3R H = sample.L.add(sample.V).normalizeLocal();
		if(N.dot(H) < 0.0_r)
		{
			H.mulLocal(-1.0_r);
		}

		const real HoV = H.dot(sample.V);
		const real NoH = N.dot(H);
		const real HoL = H.dot(sample.L);

		Vector3R F;
		Microfacet::fresnelSchlickApproximated(std::abs(HoV), sampledF0, &F);

		const real signHoV = HoV < 0.0_r ? -1.0_r : 1.0_r;
		Vector3R ior;
		m_IOR->sample(sample.X->getHitUVW(), &ior);

		// assume the outside medium has an IOR of 1.0 (which is true in most cases)
		const real iorRatio = signHoV < 0.0_r ? ior.x : 1.0_r / ior.x;
		const real sqrValue = 1.0_r - iorRatio*iorRatio*(1.0_r - HoV * HoV);

		// TIR (total internal reflection)
		if(sqrValue <= 0.0_r)
		{
			F.set(1.0_r, 1.0_r, 1.0_r);
		}

		const real D = Microfacet::normalDistributionGgxTrowbridgeReitz(NoH, alpha);
		const real G = Microfacet::geometryShadowingGgxSmith(NoV, NoL, HoV, HoL, alpha);

		// notice that the abs(N dot L) term canceled out with the lambertian term
		sample.liWeight = F.mul(D * G / (4.0_r * std::abs(NoV)));
		sample.type = ESurfaceSampleType::REFLECTION;
	}
	// refraction
	else
	{
		Vector3R sampledIor;
		m_IOR->sample(sample.X->getHitUVW(), &sampledIor);
		real iorI;
		real iorO;

		// H is on the hemisphere of N
		Vector3R H;
		if(NoL < 0.0_r)
		{
			iorI = sampledIor.x;
			iorO = 1.0_r;
		}
		else
		{
			iorI = 1.0_r;
			iorO = sampledIor.x;
		}
		H = sample.L.mul(iorI).add(sample.V.mul(iorO)).mulLocal(-1.0_r).normalizeLocal();

		const real HoV = H.dot(sample.V);
		const real NoH = N.dot(H);
		const real HoL = H.dot(sample.L);
		
		Vector3R F;
		Microfacet::fresnelSchlickApproximated(std::abs(HoV), sampledF0, &F);
		const real D = Microfacet::normalDistributionGgxTrowbridgeReitz(NoH, alpha);
		const real G = Microfacet::geometryShadowingGgxSmith(NoV, NoL, HoV, HoL, alpha);

		// notice that the abs(N dot L) term canceled out with the lambertian term
		const real dotTerm = std::abs(HoL * HoV / NoV);
		const real iorTerm = iorI / (iorI * HoL + iorO * HoV);
		sample.liWeight = F.complement().mul(D * G * dotTerm * (iorTerm * iorTerm));
		sample.type = ESurfaceSampleType::TRANSMISSION;
	}
}

}// end namespace ph