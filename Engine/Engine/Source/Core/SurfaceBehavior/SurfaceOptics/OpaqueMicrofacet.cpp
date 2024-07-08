#include "Core/SurfaceBehavior/SurfaceOptics/OpaqueMicrofacet.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"
#include "Math/constant.h"
#include "Core/SurfaceBehavior/Property/IsoTrowbridgeReitz.h"
#include "Math/math.h"
#include "Core/SurfaceBehavior/BsdfHelper.h"
#include "Core/SampleGenerator/SampleFlow.h"
#include "Math/Color/Spectrum.h"

#include <Common/assertion.h>

#include <cmath>

namespace ph
{

/*
Cook-Torrance microfacet specular BRDF is

	D(H)*F(V, H)*G(L, V, H)/(4*|NoL|*|NoV|).

A naive importance sampling strategy would be generating a microfacet normal H which
follows D(H)'s distribution, and generate L by reflecting -V using H. The PDF for this
sampling scheme is

	D(H)*|NoH|/(4*|HoL|).
	
The reason that 4*|HoL| exists is because there's a jacobian involved (from H's
probability space to L's).

The implementation is double-sided.
*/

OpaqueMicrofacet::OpaqueMicrofacet(
	const std::shared_ptr<ConductorFresnel>& fresnel,
	const std::shared_ptr<Microfacet>&       microfacet) :

	SurfaceOptics(),

	m_fresnel(fresnel),
	m_microfacet(microfacet)
{
	PH_ASSERT(fresnel && microfacet);

	m_phenomena.set(ESurfacePhenomenon::GlossyReflection);
}

ESurfacePhenomenon OpaqueMicrofacet::getPhenomenonOf(const SurfaceElemental elemental) const
{
	PH_ASSERT_EQ(elemental, 0);

	return ESurfacePhenomenon::GlossyReflection;
}

void OpaqueMicrofacet::calcBsdf(
	const BsdfQueryContext& ctx,
	const BsdfEvalInput&    in,
	BsdfEvalOutput&         out) const
{
	const math::Vector3R N = in.getX().getShadingNormal();

	const real NoL = N.dot(in.getL());
	const real NoV = N.dot(in.getV());

	// Ensure L & V lies on the same side of the surface
	if(!ctx.sidedness.isSameHemisphere(in.getX(), in.getL(), in.getV()))
	{
		out.setMeasurability(false);
		return;
	}

	math::Vector3R H;
	if(!BsdfHelper::makeHalfVectorSameHemisphere(in.getL(), in.getV(), N, &H))
	{
		out.setMeasurability(false);
		return;
	}

	const real HoL = H.dot(in.getL());

	math::Spectrum F;
	m_fresnel->calcReflectance(HoL, &F);

	const real D = m_microfacet->distribution(in.getX(), N, H);
	const real G = m_microfacet->geometry(in.getX(), N, H, in.getL(), in.getV());

	const math::Spectrum bsdf = F.mul(D * G / (4.0_r * std::abs(NoV * NoL)));
	out.setBsdf(bsdf);
}

void OpaqueMicrofacet::genBsdfSample(
	const BsdfQueryContext& ctx,
	const BsdfSampleInput&  in,
	SampleFlow&             sampleFlow,
	BsdfSampleOutput&       out) const
{
	const math::Vector3R N = in.getX().getShadingNormal();

	math::Vector3R H;
	m_microfacet->sampleH(
		in.getX(),
		N,
		sampleFlow.flow2D(),
		&H);

	const math::Vector3R L = in.getV().mul(-1.0_r).reflect(H).normalize();

	const real NoV = N.dot(in.getV());
	const real HoL = H.dot(L);
	const real dotTerms = std::abs(HoL / NoV);
	const real G = m_microfacet->geometry(in.getX(), N, H, L, in.getV());
	const real D = m_microfacet->distribution(in.getX(), N, H);

	const lta::PDF pdf = m_microfacet->pdfSampleH(in.getX(), N, H);
	PH_ASSERT(pdf.domain == lta::EDomain::HalfSolidAngle);

	math::Spectrum F;
	m_fresnel->calcReflectance(HoL, &F);

	out.setPdfAppliedBsdfCos(F.mul(G * D * dotTerms / pdf.value), N.absDot(L));
	out.setL(L);
}

void OpaqueMicrofacet::calcBsdfPdf(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	const math::Vector3R N = in.getX().getShadingNormal();

	const real NoL = N.dot(in.getL());
	const real NoV = N.dot(in.getV());

	// Ensure L & V lies on the same side of the surface
	if(!ctx.sidedness.isSameHemisphere(in.getX(), in.getL(), in.getV()))
	{
		out.setSampleDirPdf({});
		return;
	}

	math::Vector3R H;
	if(!BsdfHelper::makeHalfVectorSameHemisphere(in.getL(), in.getV(), N, &H))
	{
		out.setSampleDirPdf({});
		return;
	}

	const lta::PDF pdf = m_microfacet->pdfSampleH(in.getX(), N, H);
	PH_ASSERT(pdf.domain == lta::EDomain::HalfSolidAngle);

	// Apply the Jacobian for `HalfSolidAngle` -> `SolidAngle`
	const real pdfW = pdf.value / (4.0_r * H.absDot(in.getL()));
	out.setSampleDirPdf(lta::PDF::W(pdfW));
}

}// end namespace ph
