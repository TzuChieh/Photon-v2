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
#include <iostream>

namespace ph
{

OpaqueMicrofacet::OpaqueMicrofacet(
	const std::shared_ptr<ConductorFresnel>& fresnel,
	const std::shared_ptr<Microfacet>&       microfacet) :

	SurfaceOptics(),

	m_fresnel(fresnel),
	m_microfacet(microfacet)
{
	PH_ASSERT(fresnel && microfacet);

	m_phenomena.set({ESurfacePhenomenon::GlossyReflection});
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

	// Check if L, V lies on different side of the surface
	if(NoL * NoV <= 0.0_r)
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

	const real HoV = H.dot(in.getV());
	const real NoH = N.dot(H);
	const real HoL = H.dot(in.getL());

	math::Spectrum F;
	m_fresnel->calcReflectance(HoL, &F);

	const real D = m_microfacet->distribution(in.getX(), N, H);
	const real G = m_microfacet->shadowing(in.getX(), N, H, in.getL(), in.getV());

	const math::Spectrum bsdf = F.mul(D * G / (4.0_r * std::abs(NoV * NoL)));
	out.setBsdf(bsdf);
}

void OpaqueMicrofacet::calcBsdfSample(
	const BsdfQueryContext& ctx,
	const BsdfSampleInput&  in,
	SampleFlow&             sampleFlow,
	BsdfSampleOutput&       out) const
{
	// Cook-Torrance microfacet specular BRDF is D(H)*F(V, H)*G(L, V, H)/(4*|NoL|*|NoV|).
	// The importance sampling strategy is to generate a microfacet normal (H) which follows D(H)'s distribution, and
	// generate L by reflecting -V using H.
	// The PDF for this sampling scheme is D(H)*|NoH|/(4*|HoL|). The reason that 4*|HoL| exists is because there's a 
	// jacobian involved (from H's probability space to L's).

	const math::Vector3R N = in.getX().getShadingNormal();

	math::Vector3R H;
	m_microfacet->genDistributedH(
		in.getX(),
		N,
		sampleFlow.flow2D(),
		&H);

	const math::Vector3R L = in.getV().mul(-1.0_r).reflect(H).normalizeLocal();

	const real NoV = N.dot(in.getV());
	const real NoL = N.dot(L);
	const real HoV = H.dot(in.getV());
	const real HoL = H.dot(L);
	const real NoH = N.dot(H);

	const real multiplier = std::abs(HoL / (NoV * NoL * NoH));
	if(!std::isfinite(multiplier))
	{
		out.setMeasurability(false);
		return;
	}

	math::Spectrum F;
	m_fresnel->calcReflectance(HoL, &F);

	const real G = m_microfacet->shadowing(in.getX(), N, H, L, in.getV());
	out.setPdfAppliedBsdf(F.mul(G).mulLocal(multiplier));
	out.setL(L);
}

void OpaqueMicrofacet::calcBsdfSamplePdfW(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	const math::Vector3R N = in.getX().getShadingNormal();

	const real NoL = N.dot(in.getL());
	const real NoV = N.dot(in.getV());

	// Check if L, V lies on different side of the surface
	if(NoL * NoV <= 0.0_r)
	{
		out.setSampleDirPdfW(0);
		return;
	}

	math::Vector3R H;
	if(!BsdfHelper::makeHalfVectorSameHemisphere(in.getL(), in.getV(), N, &H))
	{
		out.setSampleDirPdfW(0);
		return;
	}

	const real NoH = N.dot(H);
	const real HoL = H.dot(in.getL());
	const real D = m_microfacet->distribution(in.getX(), N, H);

	out.setSampleDirPdfW(std::abs(D * NoH / (4.0_r * HoL)));
	if(!std::isfinite(out.getSampleDirPdfW()))
	{
		out.setSampleDirPdfW(0);
	}
}

}// end namespace ph
