#include "Core/SurfaceBehavior/SurfaceOptics/OpaqueMicrofacet.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"
#include "Math/Random.h"
#include "Math/constant.h"
#include "Core/SurfaceBehavior/Property/IsoTrowbridgeReitz.h"
#include "Core/SurfaceBehavior/Property/SchlickApproxDielectricFresnel.h"
#include "Math/Math.h"
#include "Core/Texture/TConstantTexture.h"
#include "Core/SurfaceBehavior/BsdfHelper.h"

#include <cmath>
#include <iostream>

namespace ph
{

OpaqueMicrofacet::OpaqueMicrofacet() :
	SurfaceOptics(),
	m_albedo    (std::make_shared<TConstantTexture<SpectralStrength>>(SpectralStrength(0.5_r))),
	m_microfacet(std::make_shared<IsoTrowbridgeReitz>(0.5_r)),
	m_fresnel   (std::make_shared<SchlickApproxDielectricFresnel>(1.0_r, 1.5_r))
{}

OpaqueMicrofacet::~OpaqueMicrofacet() = default;

void OpaqueMicrofacet::evalBsdf(const SurfaceHit& X, const Vector3R& L, const Vector3R& V,
                                SpectralStrength* const out_bsdf, 
                                ESurfacePhenomenon* const out_type) const
{
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
	*out_type = ESurfacePhenomenon::REFLECTION;
}

void OpaqueMicrofacet::genBsdfSample(const SurfaceHit& X, const Vector3R& V,
                                 Vector3R* const out_L, 
                                 SpectralStrength* const out_pdfAppliedBsdf, 
                                 ESurfacePhenomenon* const out_type) const
{
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

	*out_type = ESurfacePhenomenon::REFLECTION;
}

void OpaqueMicrofacet::calcBsdfSamplePdf(const SurfaceHit& X, const Vector3R& L, const Vector3R& V, const ESurfacePhenomenon& type,
                                         real* const out_pdfW) const
{
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