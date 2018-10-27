#include "Core/SurfaceBehavior/SurfaceOptics/TranslucentMicrofacet.h"
#include "Core/Texture/TConstantTexture.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"
#include "Math/Random.h"
#include "Math/constant.h"
#include "Core/SurfaceBehavior/Property/IsoTrowbridgeReitz.h"
#include "Math/math.h"
#include "Core/SurfaceBehavior/Property/SchlickApproxDielectricFresnel.h"
#include "Core/SurfaceBehavior/BsdfHelper.h"
#include "Common/assertion.h"
#include "Core/SidednessAgreement.h"

#include <memory>
#include <iostream>

namespace ph
{

TranslucentMicrofacet::TranslucentMicrofacet(
	const std::shared_ptr<DielectricFresnel>& fresnel,
	const std::shared_ptr<Microfacet>&        microfacet) :

	SurfaceOptics(),

	m_fresnel   (fresnel),
	m_microfacet(microfacet)
{
	PH_ASSERT(fresnel && microfacet);

	m_phenomena.set({ESP::GLOSSY_REFLECTION, ESP::GLOSSY_TRANSMISSION});
}

void TranslucentMicrofacet::calcBsdf(
	const BsdfEvaluation::Input& in,
	BsdfEvaluation::Output&      out,
	const SidednessAgreement&    sidedness) const
{
	const Vector3R& N = in.X.getShadingNormal();

	const real NoL = N.dot(in.L);
	const real NoV = N.dot(in.V);

	// reflection
	if(sidedness.isSameHemisphere(in.X, in.L, in.V))
	{
		Vector3R H;
		if(!BsdfHelper::makeHalfVectorSameHemisphere(in.L, in.V, N, &H))
		{
			out.bsdf.setValues(0);
			return;
		}

		const real HoV = H.dot(in.V);
		const real NoH = N.dot(H);
		const real HoL = H.dot(in.L);

		SpectralStrength F;
		m_fresnel->calcReflectance(HoL, &F);

		const real D = m_microfacet->distribution(in.X, N, H);
		const real G = m_microfacet->shadowing(in.X, N, H, in.L, in.V);

		out.bsdf = F.mul(D * G / (4.0_r * std::abs(NoV * NoL)));
	}
	// refraction
	else if(sidedness.isOppositeHemisphere(in.X, in.L, in.V))
	{
		real etaI = m_fresnel->getIorOuter();
		real etaT = m_fresnel->getIorInner();
		if(NoL < 0.0_r)
		{
			std::swap(etaI, etaT);
		}

		// H should be on the same hemisphere as N
		Vector3R H = in.L.mul(-etaI).add(in.V.mul(-etaT));
		if(H.isZero())
		{
			out.bsdf.setValues(0);
			return;
		}
		H.normalizeLocal();
		if(N.dot(H) < 0.0_r)
		{
			H.mulLocal(-1.0_r);
		}

		const real HoV = H.dot(in.V);
		const real NoH = N.dot(H);
		const real HoL = H.dot(in.L);

		SpectralStrength F;
		m_fresnel->calcTransmittance(HoL, &F);

		const real D = m_microfacet->distribution(in.X, N, H);
		const real G = m_microfacet->shadowing(in.X, N, H, in.L, in.V);

		const real dotTerm = std::abs(HoL * HoV / (NoV * NoL));
		const real iorTerm = etaI / (etaI * HoL + etaT * HoV);
		out.bsdf = F.complement().mul(D * G * dotTerm * (iorTerm * iorTerm));
	}
	else
	{
		out.bsdf.setValues(0);
	}
}

void TranslucentMicrofacet::calcBsdfSample(
	const BsdfSample::Input&  in,
	BsdfSample::Output&       out,
	const SidednessAgreement& sidedness) const
{
	out.pdfAppliedBsdf.setValues(0);

	// Cook-Torrance microfacet specular BRDF for translucent surface is:
	// |HoL||HoV|/(|NoL||NoV|)*(iorO^2)*(D(H)*F(V, H)*G(L, V, H)) / (iorI*HoL + iorO*HoV)^2.
	// The importance sampling strategy is to generate a microfacet normal (H) which follows D(H)'s distribution, and
	// generate L by reflecting/refracting -V using H.
	// The PDF for this sampling scheme is (D(H)*|NoH|) * (iorO^2 * |HoV| / ((iorI*HoL + iorO*HoV)^2)).
	// The reason that the latter multiplier in the PDF exists is because there's a jacobian involved 
	// (from H's probability space to L's).

	const Vector3R& N = in.X.getShadingNormal();

	Vector3R H;
	m_microfacet->genDistributedH(
		in.X,
		Random::genUniformReal_i0_e1(),
		Random::genUniformReal_i0_e1(), 
		N, &H);

	const real NoV = N.dot(in.V);
	const real HoV = H.dot(in.V);
	const real NoH = N.dot(H);

	SpectralStrength F;
	m_fresnel->calcReflectance(HoV, &F);

	// use Fresnel term to select which path to take and calculate L

	const real dart = Random::genUniformReal_i0_e1();
	const real reflectProb = F.avg();

	// reflect path
	if(dart < reflectProb)
	{
		// calculate reflected L
		out.L = in.V.mul(-1.0_r).reflect(H).normalizeLocal();
		if(!sidedness.isSameHemisphere(in.X, in.V, out.L))
		{
			return;
		}

		// account for probability
		F.divLocal(reflectProb);
	}
	// refract path
	else if(m_fresnel->calcRefractDir(in.V, H, &(out.L)))
	{
		if(!sidedness.isOppositeHemisphere(in.X, in.V, out.L))
		{
			return;
		}

		m_fresnel->calcTransmittance(out.L.dot(H), &F);

		// account for probability
		F.divLocal(1.0_r - reflectProb);
	}
	else
	{
		// RARE: may be called due to numerical error
		out.pdfAppliedBsdf.setValues(0);
		return;
	}

	const Vector3R& L = out.L;

	const real NoL = N.dot(L);
	const real HoL = H.dot(L);

	const real G = m_microfacet->shadowing(in.X, N, H, L, in.V);

	const real dotTerms = std::abs(HoL / (NoV * NoL * NoH));
	out.pdfAppliedBsdf.setValues(F.mul(G * dotTerms));
}

void TranslucentMicrofacet::calcBsdfSamplePdfW(
	const BsdfPdfQuery::Input& in,
	BsdfPdfQuery::Output&      out,
	const SidednessAgreement&  sidedness) const
{
	const Vector3R& N = in.X.getShadingNormal();

	// reflection
	if(sidedness.isSameHemisphere(in.X, in.L, in.V))
	{
		Vector3R H;
		if(!BsdfHelper::makeHalfVectorSameHemisphere(in.L, in.V, N, &H))
		{
			out.sampleDirPdfW = 0;
			return;
		}

		const real NoH = N.dot(H);
		const real HoL = H.dot(in.L);
		const real HoV = H.dot(in.V);
		const real D = m_microfacet->distribution(in.X, N, H);

		SpectralStrength F;
		m_fresnel->calcReflectance(HoL, &F);
		const real reflectProb = F.avg();

		out.sampleDirPdfW = std::abs(D * NoH / (4.0_r * HoL)) * reflectProb;
	}
	// refraction
	else if(sidedness.isOppositeHemisphere(in.X, in.L, in.V))
	{
		const real NoV = N.dot(in.V);
		const real NoL = N.dot(in.L);

		real etaI = m_fresnel->getIorOuter();
		real etaT = m_fresnel->getIorInner();
		if(NoL < 0.0_r)
		{
			std::swap(etaI, etaT);
		}

		// H should be on the same hemisphere as N
		Vector3R H = in.L.mul(-etaI).add(in.V.mul(-etaT));
		if(H.isZero())
		{
			out.sampleDirPdfW = 0;
			return;
		}
		H.normalizeLocal();
		if(N.dot(H) < 0.0_r)
		{
			H.mulLocal(-1.0_r);
		}

		const real HoV = H.dot(in.V);
		const real NoH = N.dot(H);
		const real HoL = H.dot(in.L);

		const real D = m_microfacet->distribution(in.X, N, H);

		SpectralStrength F;
		m_fresnel->calcReflectance(HoL, &F);
		const real refractProb = 1.0_r - F.avg();

		const real iorTerm = etaI * HoL + etaT * HoV;
		const real multiplier = (etaI * etaI * HoL) / (iorTerm * iorTerm);

		out.sampleDirPdfW = std::abs(D * NoH * multiplier) * refractProb;
	}
	else
	{
		out.sampleDirPdfW = 0.0_r;
	}
}

}// end namespace ph