#include "Core/SurfaceBehavior/SurfaceOptics/OpaqueMicrofacet.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"
#include "Math/Random.h"
#include "Math/constant.h"
#include "Core/SurfaceBehavior/Property/IsoTrowbridgeReitz.h"
#include "Core/SurfaceBehavior/Property/SchlickApproxDielectricFresnel.h"
#include "Math/math.h"
#include "Core/Texture/TConstantTexture.h"
#include "Core/SurfaceBehavior/BsdfHelper.h"
#include "Common/assertion.h"

#include <cmath>
#include <iostream>

namespace ph
{

OpaqueMicrofacet::OpaqueMicrofacet(
	const std::shared_ptr<FresnelEffect>& fresnel,
	const std::shared_ptr<Microfacet>&    microfacet) :

	SurfaceOptics(),

	//m_albedo    (std::make_shared<TConstantTexture<SpectralStrength>>(SpectralStrength(0.5_r))),
	m_fresnel   (fresnel),
	m_microfacet(microfacet)
{
	PH_ASSERT(fresnel && microfacet);

	m_phenomena.set({ESP::GLOSSY_REFLECTION});
}

void OpaqueMicrofacet::calcBsdf(
	const SurfaceHit&         X,
	const Vector3R&           L,
	const Vector3R&           V,
	const SidednessAgreement& sidedness,
	SpectralStrength* const   out_bsdf) const
{
	PH_ASSERT(out_bsdf);

	const Vector3R& N = X.getShadingNormal();

	const real NoL = N.dot(L);
	const real NoV = N.dot(V);

	// check if L, V lies on different side of the surface
	if(NoL * NoV <= 0.0_r)
	{
		out_bsdf->setValues(0);
		return;
	}

	Vector3R H;
	if(!BsdfHelper::makeHalfVectorSameHemisphere(L, V, N, &H))
	{
		out_bsdf->setValues(0);
		return;
	}

	const real HoV = H.dot(V);
	const real NoH = N.dot(H);
	const real HoL = H.dot(L);

	SpectralStrength F;
	m_fresnel->calcReflectance(HoL, &F);

	const real D = m_microfacet->distribution(X, N, H);
	const real G = m_microfacet->shadowing(X, N, H, L, V);

	*out_bsdf = F.mul(D * G / (4.0_r * std::abs(NoV * NoL)));
}

void OpaqueMicrofacet::calcBsdfSample(
	const SurfaceHit&         X,
	const Vector3R&           V,
	const SidednessAgreement& sidedness,
	Vector3R* const           out_L,
	SpectralStrength* const   out_pdfAppliedBsdf) const
{
	PH_ASSERT(out_L && out_pdfAppliedBsdf);

	// Cook-Torrance microfacet specular BRDF is D(H)*F(V, H)*G(L, V, H)/(4*|NoL|*|NoV|).
	// The importance sampling strategy is to generate a microfacet normal (H) which follows D(H)'s distribution, and
	// generate L by reflecting -V using H.
	// The PDF for this sampling scheme is D(H)*|NoH|/(4*|HoL|). The reason that 4*|HoL| exists is because there's a 
	// jacobian involved (from H's probability space to L's).

	const Vector3R& N = X.getShadingNormal();

	Vector3R H;
	m_microfacet->genDistributedH(X, 
	                              Random::genUniformReal_i0_e1(),
	                              Random::genUniformReal_i0_e1(), 
	                              N, &H);

	const Vector3R L = V.mul(-1.0_r).reflect(H).normalizeLocal();
	*out_L = L;

	const real NoV = N.dot(V);
	const real NoL = N.dot(L);
	const real HoV = H.dot(V);
	const real HoL = H.dot(L);
	const real NoH = N.dot(H);

	SpectralStrength F;
	m_fresnel->calcReflectance(HoL, &F);

	const real G = m_microfacet->shadowing(X, N, H, L, V);

	const real multiplier = std::abs(HoL / (NoV * NoL * NoH));
	out_pdfAppliedBsdf->setValues(F.mul(G).mulLocal(multiplier));
}

void OpaqueMicrofacet::calcBsdfSamplePdfW(
	const SurfaceHit&         X,
	const Vector3R&           L,
	const Vector3R&           V,
	const SidednessAgreement& sidedness,
	real* const               out_pdfW) const
{
	PH_ASSERT(out_pdfW);

	const Vector3R& N = X.getShadingNormal();

	const real NoL = N.dot(L);
	const real NoV = N.dot(V);

	// check if L, V lies on different side of the surface
	if(NoL * NoV <= 0.0_r)
	{
		*out_pdfW = 0;
		return;
	}

	Vector3R H;
	if(!BsdfHelper::makeHalfVectorSameHemisphere(L, V, N, &H))
	{
		*out_pdfW = 0;
		return;
	}

	const real NoH = N.dot(H);
	const real HoL = H.dot(L);
	const real D = m_microfacet->distribution(X, N, H);

	*out_pdfW = std::abs(D * NoH / (4.0_r * HoL));
}

}// end namespace ph