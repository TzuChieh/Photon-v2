#include "Core/SurfaceBehavior/SurfaceOptics/OpaqueMicrofacet.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"
#include "Math/Random.h"
#include "Math/constant.h"
#include "Core/SurfaceBehavior/Property/IsoTrowbridgeReitz.h"
#include "Math/math.h"
#include "Core/Texture/TConstantTexture.h"
#include "Core/SurfaceBehavior/BsdfHelper.h"
#include "Common/assertion.h"

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

	m_phenomena.set({ESurfacePhenomenon::GLOSSY_REFLECTION});
}

ESurfacePhenomenon OpaqueMicrofacet::getPhenomenonOf(const SurfaceElemental elemental) const
{
	PH_ASSERT_EQ(elemental, 0);

	return ESurfacePhenomenon::GLOSSY_REFLECTION;
}

void OpaqueMicrofacet::calcBsdf(
	const BsdfEvaluation::Input& in,
	BsdfEvaluation::Output&      out,
	const SidednessAgreement&    sidedness) const
{
	const math::Vector3R N = in.X.getShadingNormal();

	const real NoL = N.dot(in.L);
	const real NoV = N.dot(in.V);

	// check if L, V lies on different side of the surface
	if(NoL * NoV <= 0.0_r)
	{
		out.bsdf.setValues(0);
		return;
	}

	math::Vector3R H;
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

void OpaqueMicrofacet::calcBsdfSample(
	const BsdfSample::Input&  in,
	BsdfSample::Output&       out,
	const SidednessAgreement& sidedness) const
{
	// Cook-Torrance microfacet specular BRDF is D(H)*F(V, H)*G(L, V, H)/(4*|NoL|*|NoV|).
	// The importance sampling strategy is to generate a microfacet normal (H) which follows D(H)'s distribution, and
	// generate L by reflecting -V using H.
	// The PDF for this sampling scheme is D(H)*|NoH|/(4*|HoL|). The reason that 4*|HoL| exists is because there's a 
	// jacobian involved (from H's probability space to L's).

	const math::Vector3R N = in.X.getShadingNormal();

	math::Vector3R H;
	m_microfacet->genDistributedH(
		in.X,
		math::Random::genUniformReal_i0_e1(),
		math::Random::genUniformReal_i0_e1(),
		N, &H);

	const math::Vector3R L = in.V.mul(-1.0_r).reflect(H).normalizeLocal();
	out.L = L;

	const real NoV = N.dot(in.V);
	const real NoL = N.dot(L);
	const real HoV = H.dot(in.V);
	const real HoL = H.dot(L);
	const real NoH = N.dot(H);

	const real multiplier = std::abs(HoL / (NoV * NoL * NoH));
	if(!std::isfinite(multiplier))
	{
		out.setMeasurability(false);
		return;
	}

	SpectralStrength F;
	m_fresnel->calcReflectance(HoL, &F);

	const real G = m_microfacet->shadowing(in.X, N, H, L, in.V);
	out.pdfAppliedBsdf.setValues(F.mul(G).mulLocal(multiplier));
	out.setMeasurability(true);
}

void OpaqueMicrofacet::calcBsdfSamplePdfW(
	const BsdfPdfQuery::Input& in,
	BsdfPdfQuery::Output&      out,
	const SidednessAgreement&  sidedness) const
{
	const math::Vector3R N = in.X.getShadingNormal();

	const real NoL = N.dot(in.L);
	const real NoV = N.dot(in.V);

	// check if L, V lies on different side of the surface
	if(NoL * NoV <= 0.0_r)
	{
		out.sampleDirPdfW = 0;
		return;
	}

	math::Vector3R H;
	if(!BsdfHelper::makeHalfVectorSameHemisphere(in.L, in.V, N, &H))
	{
		out.sampleDirPdfW = 0;
		return;
	}

	const real NoH = N.dot(H);
	const real HoL = H.dot(in.L);
	const real D = m_microfacet->distribution(in.X, N, H);

	out.sampleDirPdfW = std::abs(D * NoH / (4.0_r * HoL));
	if(!std::isfinite(out.sampleDirPdfW))
	{
		out.sampleDirPdfW = 0;
	}
}

}// end namespace ph
