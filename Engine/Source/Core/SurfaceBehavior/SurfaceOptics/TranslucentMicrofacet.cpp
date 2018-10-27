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
	m_numElementals = 2;
}

void TranslucentMicrofacet::calcBsdf(
	const BsdfEvaluation::Input& in,
	BsdfEvaluation::Output&      out,
	const SidednessAgreement&    sidedness) const
{
	const Vector3R& N   = in.X.getShadingNormal();
	const real      NoL = N.dot(in.L);
	const real      NoV = N.dot(in.V);

	// reflection
	if(sidedness.isSameHemisphere(in.X, in.L, in.V) && 
	   (in.elemental == ALL_ELEMENTALS || in.elemental == REFLECTION))
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
	else if(sidedness.isOppositeHemisphere(in.X, in.L, in.V) &&
	        (in.elemental == ALL_ELEMENTALS || in.elemental == TRANSMISSION))
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

// Cook-Torrance microfacet specular BRDF for translucent surface is:
// |HoL||HoV|/(|NoL||NoV|)*(iorO^2)*(D(H)*F(V, H)*G(L, V, H))/(iorI*HoL+iorO*HoV)^2.
// The importance sampling strategy is to generate a microfacet normal (H) which 
// follows D(H)'s distribution, and generate L by reflecting/refracting -V using H.
// The PDF for this sampling scheme is (D(H)*|NoH|)*(iorO^2*|HoV|/((iorI*HoL+iorO*HoV)^2)).
// The reason that the latter multiplier in the PDF exists is because there's a 
// jacobian involved (from H's probability space to L's).
//
void TranslucentMicrofacet::calcBsdfSample(
	const BsdfSample::Input&  in,
	BsdfSample::Output&       out,
	const SidednessAgreement& sidedness) const
{
	const bool canReflect  = in.elemental == ALL_ELEMENTALS || in.elemental == REFLECTION;
	const bool canTransmit = in.elemental == ALL_ELEMENTALS || in.elemental == TRANSMISSION;

	if(!canReflect && !canTransmit)
	{
		out.pdfAppliedBsdf.setValues(0);
	}

	const Vector3R& N = in.X.getShadingNormal();

	Vector3R H;
	m_microfacet->genDistributedH(
		in.X,
		Random::genUniformReal_i0_e1(),
		Random::genUniformReal_i0_e1(), 
		N, &H);

	SpectralStrength F;
	m_fresnel->calcReflectance(H.dot(in.V), &F);
	const real reflectProb = F.avg();

	bool sampleReflect  = canReflect;
	bool sampleTransmit = canTransmit;

	// we cannot sample both path, choose one randomly
	if(sampleReflect && sampleTransmit)
	{
		const real dart = Random::genUniformReal_i0_e1();
		if(dart < reflectProb)
		{
			sampleTransmit = false;
		}
		else
		{
			sampleReflect = false;
		}
	}

	PH_ASSERT(sampleReflect || sampleTransmit);

	if(sampleReflect)
	{
		// calculate reflected L
		out.L = in.V.mul(-1.0_r).reflect(H).normalizeLocal();
		if(!sidedness.isSameHemisphere(in.X, in.V, out.L))
		{
			return;
		}

		// account for probability
		if(in.elemental == ALL_ELEMENTALS)
		{
			F.divLocal(reflectProb);
		}
	}
	else if(sampleTransmit && m_fresnel->calcRefractDir(in.V, H, &(out.L)))
	{
		if(!sidedness.isOppositeHemisphere(in.X, in.V, out.L))
		{
			return;
		}

		m_fresnel->calcTransmittance(out.L.dot(H), &F);

		// account for probability
		if(in.elemental == ALL_ELEMENTALS)
		{
			F.divLocal(1.0_r - reflectProb);
		}
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
	const real NoV = N.dot(in.V);
	const real NoH = N.dot(H);

	const real G        = m_microfacet->shadowing(in.X, N, H, L, in.V);
	const real dotTerms = std::abs(HoL / (NoV * NoL * NoH));
	out.pdfAppliedBsdf.setValues(F.mul(G * dotTerms));
}

void TranslucentMicrofacet::calcBsdfSamplePdfW(
	const BsdfPdfQuery::Input& in,
	BsdfPdfQuery::Output&      out,
	const SidednessAgreement&  sidedness) const
{
	const bool canReflect  = in.elemental == ALL_ELEMENTALS || in.elemental == REFLECTION;
	const bool canTransmit = in.elemental == ALL_ELEMENTALS || in.elemental == TRANSMISSION;

	const Vector3R& N = in.X.getShadingNormal();

	// reflection
	if(canReflect && sidedness.isSameHemisphere(in.X, in.L, in.V))
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
		const real D   = m_microfacet->distribution(in.X, N, H);

		SpectralStrength F;
		m_fresnel->calcReflectance(HoL, &F);
		const real reflectProb = in.elemental == ALL_ELEMENTALS ? F.avg() : 1.0_r;

		out.sampleDirPdfW = std::abs(D * NoH / (4.0_r * HoL)) * reflectProb;
	}
	// transmission
	else if(canTransmit && sidedness.isOppositeHemisphere(in.X, in.L, in.V))
	{
		const real NoV = N.dot(in.V);
		const real NoL = N.dot(in.L);

		real etaI = m_fresnel->getIorOuter();
		real etaT = m_fresnel->getIorInner();
		if(NoL < 0.0_r)
		{
			std::swap(etaI, etaT);
		}

		// here H will point into the medium with lower IoR
		// (see: B. Walter et al., Microfacet Models for Refraction, near the end of P.5)
		Vector3R H = in.L.mul(-etaI).add(in.V.mul(-etaT));
		if(H.isZero())
		{
			out.sampleDirPdfW = 0;
			return;
		}
		H.normalizeLocal();

		// make H in N's hemisphere
		if(N.dot(H) < 0.0_r)
		{
			H.mulLocal(-1.0_r);
		}

		const real HoV = H.dot(in.V);
		const real NoH = N.dot(H);
		const real HoL = H.dot(in.L);
		const real D   = m_microfacet->distribution(in.X, N, H);

		SpectralStrength F;
		m_fresnel->calcReflectance(HoL, &F);
		const real refractProb = in.elemental == ALL_ELEMENTALS ? 1.0_r - F.avg() : 1.0_r;

		const real iorTerm    = etaI * HoL + etaT * HoV;
		const real multiplier = (etaI * etaI * HoL) / (iorTerm * iorTerm);

		out.sampleDirPdfW = std::abs(D * NoH * multiplier) * refractProb;
	}
	else
	{
		out.sampleDirPdfW = 0.0_r;
	}
}

}// end namespace ph