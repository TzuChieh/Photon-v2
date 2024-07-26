#include "Core/SurfaceBehavior/SurfaceOptics/TranslucentMicrofacet.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"
#include "Math/constant.h"
#include "Math/math.h"
#include "Core/SurfaceBehavior/BsdfHelper.h"
#include "Core/LTA/SidednessAgreement.h"
#include "Core/SampleGenerator/SampleFlow.h"

#include <Common/assertion.h>

#include <memory>
#include <iostream>
#include <cmath>

namespace ph
{

/*
Cook-Torrance microfacet specular BRDF for translucent surface is

	|HoL||HoV|/(|NoL||NoV|)*(iorO^2)*(D(H)*F(V, H)*G(L, V, H))/(iorI*HoL+iorO*HoV)^2.

A naive importance sampling strategy would be generating a microfacet normal H which
follows D(H)'s distribution, and generate L by reflecting/refracting -V using H.
The PDF for this sampling scheme is

	(D(H)*|NoH|)*(iorO^2*|HoL|/((iorI*HoL+iorO*HoV)^2)).

The reason that the latter multiplier in the PDF exists is because there's a
jacobian involved (from H's probability space to L's). More sophisticated sampling
strategy typically follow this pattern, with a better distributed H.
*/

TranslucentMicrofacet::TranslucentMicrofacet(
	const std::shared_ptr<DielectricFresnel>& fresnel,
	const std::shared_ptr<Microfacet>&        microfacet) :

	SurfaceOptics(),

	m_fresnel   (fresnel),
	m_microfacet(microfacet)
{
	PH_ASSERT(fresnel);
	PH_ASSERT(microfacet);

	m_phenomena.set({ESurfacePhenomenon::GlossyReflection, ESurfacePhenomenon::GlossyTransmission});
	m_numElementals = 2;
}

ESurfacePhenomenon TranslucentMicrofacet::getPhenomenonOf(const SurfaceElemental elemental) const
{
	PH_ASSERT_LT(elemental, 2);

	return elemental == REFLECTION ? ESurfacePhenomenon::GlossyReflection : 
	                                 ESurfacePhenomenon::GlossyTransmission;
}

void TranslucentMicrofacet::calcBsdf(
	const BsdfQueryContext& ctx,
	const BsdfEvalInput&    in,
	BsdfEvalOutput&         out) const
{
	const math::Vector3R N   = in.getX().getShadingNormal();
	const real           NoL = N.dot(in.getL());
	const real           NoV = N.dot(in.getV());

	const real NoLmulNoV = NoL * NoV;
	if(NoLmulNoV == 0.0_r)
	{
		out.setMeasurability(false);
		return;
	}

	// Reflection
	if(ctx.sidedness.isSameHemisphere(in.getX(), in.getL(), in.getV()) &&
	   (ctx.elemental == ALL_SURFACE_ELEMENTALS || ctx.elemental == REFLECTION))
	{
		math::Vector3R H;
		if(!BsdfHelper::makeHalfVectorSameHemisphere(in.getL(), in.getV(), N, &H))
		{
			out.setMeasurability(false);
			return;
		}

		const real HoV = H.dot(in.getV());
		const real NoH = N.dot(H);
		const real HoL = H.dot(in.getL());

		const math::Spectrum F = m_fresnel->calcReflectance(HoL);
		const real D = m_microfacet->distribution(in.getX(), N, H);
		const real G = m_microfacet->geometry(in.getX(), N, H, in.getL(), in.getV());

		const math::Spectrum bsdf = F.mul(D * G / (4.0_r * std::abs(NoLmulNoV)));
		out.setBsdf(bsdf);
	}
	// Refraction
	else if(ctx.sidedness.isOppositeHemisphere(in.getX(), in.getL(), in.getV()) &&
	        (ctx.elemental == ALL_SURFACE_ELEMENTALS || ctx.elemental == TRANSMISSION))
	{
		real etaI = m_fresnel->getIorOuter();
		real etaT = m_fresnel->getIorInner();
		if(NoL < 0.0_r)
		{
			std::swap(etaI, etaT);
		}

		// H should be on the same hemisphere as N
		math::Vector3R H = in.getL().mul(-etaI).add(in.getV().mul(-etaT));
		if(H.isZero())
		{
			out.setMeasurability(false);
			return;
		}
		H.normalizeLocal();
		if(N.dot(H) < 0.0_r)
		{
			H.mulLocal(-1.0_r);
		}

		const real HoV = H.dot(in.getV());
		const real NoH = N.dot(H);
		const real HoL = H.dot(in.getL());

		const math::Spectrum F = m_fresnel->calcTransmittance(HoL);
		const real D = m_microfacet->distribution(in.getX(), N, H);
		const real G = m_microfacet->geometry(in.getX(), N, H, in.getL(), in.getV());

		// Account for non-symmetric scattering due to solid angle compression/expansion
		const real transportFactor = ctx.transport == lta::ETransport::Radiance ?
			etaT / etaI : 1.0_r;

		const real iorTerm = transportFactor * etaI / (etaI * HoL + etaT * HoV);
		if(!std::isfinite(iorTerm))
		{
			out.setMeasurability(false);
			return;
		}

		const real dotTerm = std::abs(HoL * HoV / NoLmulNoV);

		const math::Spectrum bsdf = F.mul(D * G * dotTerm * (iorTerm * iorTerm));
		out.setBsdf(bsdf);
	}
	else
	{
		out.setMeasurability(false);
	}
}

void TranslucentMicrofacet::genBsdfSample(
	const BsdfQueryContext& ctx,
	const BsdfSampleInput&  in,
	SampleFlow&             sampleFlow,
	BsdfSampleOutput&       out) const
{
	const bool canReflect  = ctx.elemental == ALL_SURFACE_ELEMENTALS || ctx.elemental == REFLECTION;
	const bool canTransmit = ctx.elemental == ALL_SURFACE_ELEMENTALS || ctx.elemental == TRANSMISSION;

	if(!canReflect && !canTransmit)
	{
		out.setMeasurability(false);
		return;
	}

	const math::Vector3R N = in.getX().getShadingNormal();

	math::Vector3R H;
	m_microfacet->sampleVisibleH(
		in.getX(),
		N,
		in.getV(),
		sampleFlow.flow2D(),
		&H);

	math::Spectrum F = m_fresnel->calcReflectance(H.dot(in.getV()));
	const real reflectProb = getReflectionProbability(F);

	bool sampleReflect  = canReflect;
	bool sampleTransmit = canTransmit;

	// We cannot sample both paths, choose one randomly
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

	math::Vector3R L;
	if(sampleReflect)
	{
		// Calculate reflected L
		L = in.getV().mul(-1.0_r).reflect(H).normalizeLocal();
		if(!ctx.sidedness.isSameHemisphere(in.getX(), in.getV(), L))
		{
			out.setMeasurability(false);
			return;
		}

		// Account for pick probability
		if(ctx.elemental == ALL_SURFACE_ELEMENTALS)
		{
			F.divLocal(reflectProb);
		}
	}
	else if(sampleTransmit)
	{
		// TIR is already handled by the path probability, this only checks for bad configurations
		const auto optRefractDir = m_fresnel->calcRefractDir(in.getV(), H);
		if(!optRefractDir || !ctx.sidedness.isOppositeHemisphere(in.getX(), in.getV(), *optRefractDir))
		{
			out.setMeasurability(false);
			return;
		}

		L = *optRefractDir;
		F = m_fresnel->calcTransmittance(H.dot(L));

		real etaI = m_fresnel->getIorOuter();
		real etaT = m_fresnel->getIorInner();
		if(N.dot(L) < 0.0_r)
		{
			std::swap(etaI, etaT);
		}

		const real relativeIor = etaT / etaI;
		out.setRelativeIor(relativeIor);

		// Account for non-symmetric scattering due to solid angle compression/expansion
		if(ctx.transport == lta::ETransport::Radiance)
		{
			F.mulLocal(relativeIor * relativeIor);
		}

		// Account for pick probability
		if(ctx.elemental == ALL_SURFACE_ELEMENTALS)
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

	// Note: Reflection case shares the same final formula as refraction case. The `dotTerms`
	// for reflection case is `std::abs(HoL / NoV) == std::abs(HoV / NoV)` since `HoL == HoV`.

	const lta::PDF pdf = m_microfacet->pdfSampleVisibleH(in.getX(), N, H, in.getV());
	PH_ASSERT(pdf.domain == lta::EDomain::HalfSolidAngle);

	const real NoV = N.dot(in.getV());
	const real HoV = H.dot(in.getV());
	const real dotTerms = std::abs(HoV / NoV);
	const real G = m_microfacet->geometry(in.getX(), N, H, L, in.getV());
	const real D = m_microfacet->distribution(in.getX(), N, H);

	out.setPdfAppliedBsdfCos(F.mul(G * D * dotTerms / pdf.value), N.absDot(L));
	out.setL(L);
}

void TranslucentMicrofacet::calcBsdfPdf(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	const bool canReflect  = ctx.elemental == ALL_SURFACE_ELEMENTALS || ctx.elemental == REFLECTION;
	const bool canTransmit = ctx.elemental == ALL_SURFACE_ELEMENTALS || ctx.elemental == TRANSMISSION;

	const math::Vector3R N = in.getX().getShadingNormal();

	real sampleDirPdfW = 0;

	// Reflection
	if(canReflect && ctx.sidedness.isSameHemisphere(in.getX(), in.getL(), in.getV()))
	{
		math::Vector3R H;
		if(!BsdfHelper::makeHalfVectorSameHemisphere(in.getL(), in.getV(), N, &H))
		{
			out.setSampleDirPdf({});
			return;
		}

		const math::Spectrum F = m_fresnel->calcReflectance(H.dot(in.getL()));
		const real reflectProb = ctx.elemental == ALL_SURFACE_ELEMENTALS
			? getReflectionProbability(F)
			: 1.0_r;

		const lta::PDF pdf = m_microfacet->pdfSampleVisibleH(in.getX(), N, H, in.getV());
		PH_ASSERT(pdf.domain == lta::EDomain::HalfSolidAngle);

		// Apply the Jacobian for `HalfSolidAngle` -> `SolidAngle` and path selection probability
		sampleDirPdfW = pdf.value / (4.0_r * H.absDot(in.getL())) * reflectProb;
	}
	// Transmission
	else if(canTransmit && ctx.sidedness.isOppositeHemisphere(in.getX(), in.getL(), in.getV()))
	{
		const real NoV = N.dot(in.getV());
		const real NoL = N.dot(in.getL());

		real etaI = m_fresnel->getIorOuter();
		real etaT = m_fresnel->getIorInner();
		if(NoL < 0.0_r)
		{
			std::swap(etaI, etaT);
		}

		// Here H will point into the medium with lower IoR
		// (see: B. Walter et al., Microfacet Models for Refraction, near the end of P.5)
		math::Vector3R H = in.getL().mul(-etaI).add(in.getV().mul(-etaT));
		if(H.isZero())
		{
			out.setSampleDirPdf({});
			return;
		}
		H.normalizeLocal();

		// Make H in N's hemisphere
		if(N.dot(H) < 0.0_r)
		{
			H.mulLocal(-1.0_r);
		}

		const real HoV = H.dot(in.getV());
		const real HoL = H.dot(in.getL());
		const real iorTerm = etaI * HoL + etaT * HoV;
		const real multiplier = std::abs(etaI * etaI * HoL) / (iorTerm * iorTerm);

		const math::Spectrum F = m_fresnel->calcReflectance(HoL);
		const real refractProb = ctx.elemental == ALL_SURFACE_ELEMENTALS
			? 1.0_r - getReflectionProbability(F)
			: 1.0_r;

		const lta::PDF pdf = m_microfacet->pdfSampleVisibleH(in.getX(), N, H, in.getV());
		PH_ASSERT(pdf.domain == lta::EDomain::HalfSolidAngle);

		// Apply the Jacobian for `HalfSolidAngle` -> `SolidAngle` and path selection probability
		sampleDirPdfW = pdf.value * multiplier * refractProb;
	}
	else
	{
		sampleDirPdfW = 0;
	}

	out.setSampleDirPdf(lta::PDF::W(sampleDirPdfW));
}

real TranslucentMicrofacet::getReflectionProbability(const math::Spectrum& F)
{
	constexpr real minProbability = 0.0001_r;

	return math::clamp(F.avg(), minProbability, 1.0_r - minProbability);
}

}// end namespace ph
