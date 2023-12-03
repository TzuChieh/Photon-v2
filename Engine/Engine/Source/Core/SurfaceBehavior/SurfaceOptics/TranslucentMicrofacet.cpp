#include "Core/SurfaceBehavior/SurfaceOptics/TranslucentMicrofacet.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"
#include "Math/constant.h"
#include "Math/math.h"
#include "Core/SurfaceBehavior/BsdfHelper.h"
#include "Common/assertion.h"
#include "Core/LTABuildingBlock/SidednessAgreement.h"
#include "Core/SampleGenerator/SampleFlow.h"

#include <memory>
#include <iostream>
#include <cmath>

namespace ph
{

TranslucentMicrofacet::TranslucentMicrofacet(
	const std::shared_ptr<DielectricFresnel>& fresnel,
	const std::shared_ptr<Microfacet>&        microfacet) :

	SurfaceOptics(),

	m_fresnel   (fresnel),
	m_microfacet(microfacet)
{
	PH_ASSERT(fresnel);
	PH_ASSERT(microfacet);

	m_phenomena.set({ESurfacePhenomenon::GLOSSY_REFLECTION, ESurfacePhenomenon::GLOSSY_TRANSMISSION});
	m_numElementals = 2;
}

ESurfacePhenomenon TranslucentMicrofacet::getPhenomenonOf(const SurfaceElemental elemental) const
{
	PH_ASSERT_LT(elemental, 2);

	return elemental == REFLECTION ? ESurfacePhenomenon::GLOSSY_REFLECTION : 
	                                 ESurfacePhenomenon::GLOSSY_TRANSMISSION;
}

void TranslucentMicrofacet::calcBsdf(
	const BsdfQueryContext& ctx,
	const BsdfEvalInput&    in,
	BsdfEvalOutput&         out) const
{
	const math::Vector3R N   = in.X.getShadingNormal();
	const real           NoL = N.dot(in.L);
	const real           NoV = N.dot(in.V);

	const real NoLmulNoV = NoL * NoV;
	if(NoLmulNoV == 0.0_r)
	{
		out.bsdf.setColorValues(0);
		return;
	}

	// reflection
	if(ctx.sidedness.isSameHemisphere(in.X, in.L, in.V) && 
	   (ctx.elemental == ALL_ELEMENTALS || ctx.elemental == REFLECTION))
	{
		math::Vector3R H;
		if(!BsdfHelper::makeHalfVectorSameHemisphere(in.L, in.V, N, &H))
		{
			out.bsdf.setColorValues(0);
			return;
		}

		const real HoV = H.dot(in.V);
		const real NoH = N.dot(H);
		const real HoL = H.dot(in.L);

		math::Spectrum F;
		m_fresnel->calcReflectance(HoL, &F);

		const real D = m_microfacet->distribution(in.X, N, H);
		const real G = m_microfacet->shadowing(in.X, N, H, in.L, in.V);

		out.bsdf = F.mul(D * G / (4.0_r * std::abs(NoLmulNoV)));
	}
	// refraction
	else if(ctx.sidedness.isOppositeHemisphere(in.X, in.L, in.V) &&
	        (ctx.elemental == ALL_ELEMENTALS || ctx.elemental == TRANSMISSION))
	{
		real etaI = m_fresnel->getIorOuter();
		real etaT = m_fresnel->getIorInner();
		if(NoL < 0.0_r)
		{
			std::swap(etaI, etaT);
		}

		// H should be on the same hemisphere as N
		math::Vector3R H = in.L.mul(-etaI).add(in.V.mul(-etaT));
		if(H.isZero())
		{
			out.bsdf.setColorValues(0);
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

		math::Spectrum F;
		m_fresnel->calcTransmittance(HoL, &F);

		const real D = m_microfacet->distribution(in.X, N, H);
		const real G = m_microfacet->shadowing(in.X, N, H, in.L, in.V);

		const real transportFactor = ctx.transport == ETransport::RADIANCE ?
			etaT / etaI : 1.0_r;

		const real iorTerm = transportFactor * etaI / (etaI * HoL + etaT * HoV);
		if(!std::isfinite(iorTerm))
		{
			out.bsdf.setColorValues(0);
			return;
		}

		const real dotTerm = std::abs(HoL * HoV / NoLmulNoV);

		out.bsdf = F.mul(D * G * dotTerm * (iorTerm * iorTerm));
	}
	else
	{
		out.bsdf.setColorValues(0);
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
	const BsdfQueryContext& ctx,
	const BsdfSampleInput&  in,
	SampleFlow&             sampleFlow,
	BsdfSampleOutput&       out) const
{
	const bool canReflect  = ctx.elemental == ALL_ELEMENTALS || ctx.elemental == REFLECTION;
	const bool canTransmit = ctx.elemental == ALL_ELEMENTALS || ctx.elemental == TRANSMISSION;

	if(!canReflect && !canTransmit)
	{
		out.setMeasurability(false);
		return;
	}

	const math::Vector3R N = in.X.getShadingNormal();

	math::Vector3R H;
	m_microfacet->genDistributedH(
		in.X,
		N,
		sampleFlow.flow2D(),
		&H);

	math::Spectrum F;
	m_fresnel->calcReflectance(H.dot(in.V), &F);
	const real reflectProb = getReflectionProbability(F);

	bool sampleReflect  = canReflect;
	bool sampleTransmit = canTransmit;

	// we cannot sample both path, choose one randomly
	if(sampleReflect && sampleTransmit)
	{
		if(sampleFlow.unflowedPick(reflectProb))
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
		if(!ctx.sidedness.isSameHemisphere(in.X, in.V, out.L))
		{
			out.setMeasurability(false);
			return;
		}

		// account for probability
		if(ctx.elemental == ALL_ELEMENTALS)
		{
			F.divLocal(reflectProb);
		}
	}
	else if(sampleTransmit && m_fresnel->calcRefractDir(in.V, H, &(out.L)))
	{
		if(!ctx.sidedness.isOppositeHemisphere(in.X, in.V, out.L))
		{
			out.setMeasurability(false);
			return;
		}

		m_fresnel->calcTransmittance(H.dot(out.L), &F);

		if(ctx.transport == ETransport::RADIANCE)
		{
			real etaI = m_fresnel->getIorOuter();
			real etaT = m_fresnel->getIorInner();
			if(N.dot(out.L) < 0.0_r)
			{
				std::swap(etaI, etaT);
			}
			F.mulLocal(etaT * etaT / (etaI * etaI));
		}

		// account for probability
		if(ctx.elemental == ALL_ELEMENTALS)
		{
			F.divLocal(1.0_r - reflectProb);
		}
	}
	else
	{
		// RARE: may be called due to numerical error
		out.setMeasurability(false);
		return;
	}

	const math::Vector3R L = out.L;

	const real NoL = N.dot(L);
	const real HoV = H.dot(in.V);
	const real NoV = N.dot(in.V);
	const real NoH = N.dot(H);

	const real G        = m_microfacet->shadowing(in.X, N, H, L, in.V);
	const real dotTerms = std::abs(HoV / (NoV * NoL * NoH));
	if(!std::isfinite(dotTerms))
	{
		out.setMeasurability(false);
		return;
	}

	out.pdfAppliedBsdf = F.mul(G * dotTerms);
	out.setMeasurability(true);
}

void TranslucentMicrofacet::calcBsdfSamplePdfW(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	const bool canReflect  = ctx.elemental == ALL_ELEMENTALS || ctx.elemental == REFLECTION;
	const bool canTransmit = ctx.elemental == ALL_ELEMENTALS || ctx.elemental == TRANSMISSION;

	const math::Vector3R N = in.X.getShadingNormal();

	// reflection
	if(canReflect && ctx.sidedness.isSameHemisphere(in.X, in.L, in.V))
	{
		math::Vector3R H;
		if(!BsdfHelper::makeHalfVectorSameHemisphere(in.L, in.V, N, &H))
		{
			out.sampleDirPdfW = 0;
			return;
		}

		const real HoL = H.dot(in.L);
		const real NoH = N.dot(H);
		const real HoV = H.dot(in.V);
		const real D   = m_microfacet->distribution(in.X, N, H);

		math::Spectrum F;
		m_fresnel->calcReflectance(HoL, &F);
		const real reflectProb = ctx.elemental == ALL_ELEMENTALS ? getReflectionProbability(F) : 1.0_r;

		out.sampleDirPdfW = std::abs(D * NoH / (4.0_r * HoL)) * reflectProb;
	}
	// transmission
	else if(canTransmit && ctx.sidedness.isOppositeHemisphere(in.X, in.L, in.V))
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
		math::Vector3R H = in.L.mul(-etaI).add(in.V.mul(-etaT));
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

		math::Spectrum F;
		m_fresnel->calcReflectance(HoL, &F);
		const real refractProb = ctx.elemental == ALL_ELEMENTALS ? 1.0_r - getReflectionProbability(F) : 1.0_r;

		const real iorTerm    = etaI * HoL + etaT * HoV;
		const real multiplier = (etaI * etaI * HoL) / (iorTerm * iorTerm);

		out.sampleDirPdfW = std::abs(D * NoH * multiplier) * refractProb;
	}
	else
	{
		out.sampleDirPdfW = 0.0_r;
	}

	if(!std::isfinite(out.sampleDirPdfW))
	{
		out.sampleDirPdfW = 0;
	}
}

real TranslucentMicrofacet::getReflectionProbability(const math::Spectrum& F)
{
	constexpr real MIN_PROBABILITY = 0.0001_r;

	return math::clamp(F.avg(), MIN_PROBABILITY, 1.0_r - MIN_PROBABILITY);
}

}// end namespace ph