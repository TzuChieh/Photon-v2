#include "Core/SurfaceBehavior/BSDF/TranslucentMicrofacet.h"
#include "Actor/Texture/ConstantTexture.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"
#include "Math/Random.h"
#include "Math/constant.h"
#include "Core/Intersection.h"
#include "Core/SurfaceBehavior/BSDF/random_sample.h"
#include "Core/SurfaceBehavior/BSDF/Microfacet.h"
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

void TranslucentMicrofacet::evaluate(const Intersection& X, const Vector3R& L, const Vector3R& V,
                                     SpectralStrength* const out_bsdf, ESurfacePhenomenon* const out_type) const
{
	const Vector3R& N = X.getHitSmoothNormal();

	const real NoL = N.dot(L);
	const real NoV = N.dot(V);

	SpectralStrength sampledAlpha;// FIXME
	m_alpha->sample(X.getHitUVW(), &sampledAlpha);
	const real alpha = sampledAlpha.genRgb().x;
	SpectralStrength sampledF0;
	m_F0->sample(X.getHitUVW(), &sampledF0);

	// reflection
	if(NoL * NoV >= 0.0_r)
	{
		// H is on the hemisphere of N
		Vector3R H = L.add(V).normalizeLocal();
		if(N.dot(H) < 0.0_r)
		{
			H.mulLocal(-1.0_r);
		}

		const real HoV = H.dot(V);
		const real NoH = N.dot(H);
		const real HoL = H.dot(L);

		SpectralStrength F;
		Microfacet::fresnelSchlickApproximated(std::abs(HoV), sampledF0, &F);

		const real signHoV = HoV < 0.0_r ? -1.0_r : 1.0_r;
		SpectralStrength iorss;// FIXME
		m_IOR->sample(X.getHitUVW(), &iorss);
		Vector3R ior = iorss.genRgb();

		// assume the outside medium has an IOR of 1.0 (which is true in most cases)
		const real iorRatio = signHoV < 0.0_r ? ior.x : 1.0_r / ior.x;
		const real sqrValue = 1.0_r - iorRatio*iorRatio*(1.0_r - HoV * HoV);

		// TIR (total internal reflection)
		if(sqrValue <= 0.0_r)
		{
			F.set(1.0_r);
		}

		const real D = Microfacet::normalDistributionGgxTrowbridgeReitz(NoH, alpha);
		const real G = Microfacet::geometryShadowingGgxSmith(NoV, NoL, HoV, HoL, alpha);

		*out_bsdf = F.mul(D * G / (4.0_r * std::abs(NoV * NoL)));
		*out_type = ESurfacePhenomenon::REFLECTION;
	}
	// refraction
	else
	{
		SpectralStrength sampledIorss;// FIXME
		m_IOR->sample(X.getHitUVW(), &sampledIorss);
		Vector3R sampledIor = sampledIorss.genRgb();
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
		H = L.mul(iorI).add(V.mul(iorO)).mulLocal(-1.0_r).normalizeLocal();

		const real HoV = H.dot(V);
		const real NoH = N.dot(H);
		const real HoL = H.dot(L);

		SpectralStrength F;
		Microfacet::fresnelSchlickApproximated(std::abs(HoV), sampledF0, &F);
		const real D = Microfacet::normalDistributionGgxTrowbridgeReitz(NoH, alpha);
		const real G = Microfacet::geometryShadowingGgxSmith(NoV, NoL, HoV, HoL, alpha);

		const real dotTerm = std::abs(HoL * HoV / (NoV * NoL));
		const real iorTerm = iorI / (iorI * HoL + iorO * HoV);
		*out_bsdf = F.complement().mul(D * G * dotTerm * (iorTerm * iorTerm));
		*out_type = ESurfacePhenomenon::TRANSMISSION;
	}
}

void TranslucentMicrofacet::genSample(const Intersection& X, const Vector3R& V,
                                      Vector3R* const out_L, SpectralStrength* const out_pdfAppliedBsdf, ESurfacePhenomenon* const out_type) const
{
	// Cook-Torrance microfacet specular BRDF for translucent surface is:
	// |HoL||HoV|/(|NoL||NoV|)*(iorO^2)*(D(H)*F(V, H)*G(L, V, H)) / (iorI*HoL + iorO*HoV)^2.
	// The importance sampling strategy is to generate a microfacet normal (H) which follows D(H)'s distribution, and
	// generate L by reflecting/refracting -V using H.
	// The PDF for this sampling scheme is (D(H)*|NoH|) * (iorO^2 * |HoV| / ((iorI*HoL + iorO*HoV)^2)).
	// The reason that the latter multiplier in the PDF exists is because there's a jacobian involved 
	// (from H's probability space to L's).

	SpectralStrength sampledAlpha;// FIXME
	m_alpha->sample(X.getHitUVW(), &sampledAlpha);
	const real alpha = sampledAlpha.genRgb().x;

	SpectralStrength sampledF0;
	m_F0->sample(X.getHitUVW(), &sampledF0);

	const Vector3R& N = X.getHitSmoothNormal();
	Vector3R H;

	genUnitHemisphereGgxTrowbridgeReitzNdfSample(Random::genUniformReal_i0_e1(), Random::genUniformReal_i0_e1(), alpha, &H);
	Vector3R u;
	Vector3R v(N);
	Vector3R w;
	Math::formOrthonormalBasis(v, &u, &w);
	H = u.mulLocal(H.x).addLocal(v.mulLocal(H.y)).addLocal(w.mulLocal(H.z));
	H.normalizeLocal();

	const real NoV = N.dot(V);
	const real HoV = H.dot(V);
	const real NoH = N.dot(H);

	SpectralStrength F;
	Microfacet::fresnelSchlickApproximated(abs(HoV), sampledF0, &F);

	// use Fresnel term to select which path to take and calculate L

	const real dart = Random::genUniformReal_i0_e1();
	const real reflectProb = F.avg();

	// reflect path
	if(dart < reflectProb)
	{
		// calculate reflected L
		*out_L = V.mul(-1.0_r).reflect(H).normalizeLocal();

		// account for probability
		F.divLocal(reflectProb);

		*out_type = ESurfacePhenomenon::REFLECTION;
	}
	// refract path
	else
	{
		real signHoV = HoV < 0.0_r ? -1.0_r : 1.0_r;
		SpectralStrength iorss;// FIXME
		m_IOR->sample(X.getHitUVW(), &iorss);
		Vector3R ior = iorss.genRgb();

		// assume the outside medium has an IOR of 1.0 (which is true in most cases)
		const real iorRatio = signHoV < 0.0_r ? ior.x : 1.0_r / ior.x;
		const real sqrValue = 1.0_r - iorRatio*iorRatio*(1.0_r - HoV * HoV);

		// TIR (total internal reflection)
		if(sqrValue <= 0.0_r)
		{
			// calculate reflected L
			*out_L = V.mul(-1.0_r).reflectLocal(H).normalizeLocal();

			*out_type = ESurfacePhenomenon::REFLECTION;

			// account for probability
			F = F.complement().divLocal(1.0_r - reflectProb);
		}
		// refraction
		else
		{
			// calculate refracted L
			const real Hfactor = iorRatio * HoV - signHoV * std::sqrt(sqrValue);
			const real Vfactor = -iorRatio;
			*out_L = H.mul(Hfactor).addLocal(V.mul(Vfactor)).normalizeLocal();

			*out_type = ESurfacePhenomenon::TRANSMISSION;

			// account for probability
			F = F.complement().divLocal(1.0_r - reflectProb);
		}
	}

	const Vector3R& L = *out_L;

	const real NoL = N.dot(L);
	const real HoL = H.dot(L);

	const real G = Microfacet::geometryShadowingGgxSmith(NoV, NoL, HoV, HoL, alpha);

	const real dotTerms = std::abs(HoL / (NoV * NoL * NoH));
	out_pdfAppliedBsdf->set(F.mul(G * dotTerms));
}

void TranslucentMicrofacet::calcSampleDirPdfW(const Intersection& X, const Vector3R& L, const Vector3R& V, const ESurfacePhenomenon& type,
                                              real* const out_pdfW) const
{
	const Vector3R& N = X.getHitSmoothNormal();
	const real NoL = N.dot(L);

	SpectralStrength sampledAlpha;// FIXME
	m_alpha->sample(X.getHitUVW(), &sampledAlpha);
	const real alpha = sampledAlpha.genRgb().x;

	SpectralStrength sampledF0;
	m_F0->sample(X.getHitUVW(), &sampledF0);

	switch(type)
	{
	case ESurfacePhenomenon::REFLECTION:
	{
		// H is on the hemisphere of N
		Vector3R H = L.add(V).normalizeLocal();
		if(NoL < 0.0_r)
		{
			H.mulLocal(-1.0_r);
		}

		const real NoH = N.dot(H);
		const real HoL = H.dot(L);
		const real HoV = H.dot(V);
		const real D = Microfacet::normalDistributionGgxTrowbridgeReitz(NoH, alpha);

		SpectralStrength F;
		Microfacet::fresnelSchlickApproximated(abs(HoV), sampledF0, &F);
		real reflectProb = F.avg();

		const real signHoV = HoV < 0.0_r ? -1.0_r : 1.0_r;
		SpectralStrength iorss;// FIXME
		m_IOR->sample(X.getHitUVW(), &iorss);
		Vector3R ior = iorss.genRgb();

		// assume the outside medium has an IOR of 1.0 (which is true in most cases)
		const real iorRatio = signHoV < 0.0_r ? ior.x : 1.0_r / ior.x;
		const real sqrValue = 1.0_r - iorRatio*iorRatio*(1.0_r - HoV * HoV);

		// TIR (total internal reflection)
		if(sqrValue <= 0.0_r)
		{
			reflectProb = 1.0_r;
		}

		*out_pdfW = std::abs(D * NoH / (4.0_r * HoL)) * reflectProb;
		break;
	}

	case ESurfacePhenomenon::TRANSMISSION:
	{
		SpectralStrength sampledIorss;// FIXME
		m_IOR->sample(X.getHitUVW(), &sampledIorss);
		Vector3R sampledIor = sampledIorss.genRgb();
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
		H = L.mul(iorI).add(V.mul(iorO)).mulLocal(-1.0_r).normalizeLocal();

		const real HoV = H.dot(V);
		const real NoH = N.dot(H);
		const real HoL = H.dot(L);

		const real D = Microfacet::normalDistributionGgxTrowbridgeReitz(NoH, alpha);

		SpectralStrength F;
		Microfacet::fresnelSchlickApproximated(abs(HoV), sampledF0, &F);
		const real reflectProb = 1.0_r - F.avg();

		const real iorTerm = iorI*HoL + iorO*HoV;
		const real multiplier = (iorI * iorI * HoV) / (iorTerm * iorTerm);

		*out_pdfW = std::abs(D * NoH * multiplier) * reflectProb;
		break;
	}

	default:
		std::cerr << "warning: at TranslucentMicrofacet::calcSampleDirPdfW(), invalid phenomenon type detected" << std::endl;
		*out_pdfW = 0.0_r;
		break;
	}
}

}// end namespace ph