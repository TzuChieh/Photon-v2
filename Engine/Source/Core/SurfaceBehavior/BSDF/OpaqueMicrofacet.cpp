#include "Core/SurfaceBehavior/BSDF/OpaqueMicrofacet.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"
#include "Math/Random.h"
#include "Math/constant.h"
#include "Core/Intersection.h"
#include "Core/SurfaceBehavior/BSDF/random_sample.h"
#include "Core/SurfaceBehavior/Utility/TrowbridgeReitz.h"
#include "Core/SurfaceBehavior/Utility/SchlickApproxDielectricFresnel.h"
#include "Math/Math.h"

#include <cmath>
#include <iostream>

namespace ph
{

OpaqueMicrofacet::OpaqueMicrofacet() :
	m_albedo    (std::make_shared<ConstantTexture>(Vector3R(0.5_r,  0.5_r,  0.5_r))),
	m_microfacet(std::make_shared<TrowbridgeReitz>(0.5_r)),
	m_fresnel   (std::make_shared<SchlickApproxDielectricFresnel>(1.0_r, 1.5_r))
{

}

OpaqueMicrofacet::~OpaqueMicrofacet() = default;

void OpaqueMicrofacet::evaluate(const Intersection& X, const Vector3R& L, const Vector3R& V,
                                SpectralStrength* const out_bsdf, 
                                ESurfacePhenomenon* const out_type) const
{
	const Vector3R& N = X.getHitSmoothNormal();

	const real NoL = N.dot(L);
	const real NoV = N.dot(V);

	// check if L, V lies on different side of the surface
	if(NoL * NoV <= 0.0_r)
	{
		out_bsdf->set(0);
		return;
	}

	// H is on the hemisphere of N
	Vector3R H = L.add(V).normalizeLocal();
	if(NoL < 0.0_r)
	{
		H.mulLocal(-1.0_r);
	}

	const real HoV = H.dot(V);
	const real NoH = N.dot(H);
	const real HoL = H.dot(L);

	SpectralStrength F;
	m_fresnel->calcReflectance(HoL, &F);

	const real D = m_microfacet->distribution(N, H);
	const real G = m_microfacet->shadowing(N, H, L, V);

	*out_bsdf = F.mul(D * G / (4.0_r * std::abs(NoV * NoL)));
	*out_type = ESurfacePhenomenon::REFLECTION;
}

void OpaqueMicrofacet::genSample(const Intersection& X, const Vector3R& V,
                                 Vector3R* const out_L, 
                                 SpectralStrength* const out_pdfAppliedBsdf, 
                                 ESurfacePhenomenon* const out_type) const
{
	// Cook-Torrance microfacet specular BRDF is D(H)*F(V, H)*G(L, V, H)/(4*|NoL|*|NoV|).
	// The importance sampling strategy is to generate a microfacet normal (H) which follows D(H)'s distribution, and
	// generate L by reflecting -V using H.
	// The PDF for this sampling scheme is D(H)*|NoH|/(4*|HoL|). The reason that 4*|HoL| exists is because there's a 
	// jacobian involved (from H's probability space to L's).

	const Vector3R& N = X.getHitSmoothNormal();

	Vector3R H;
	m_microfacet->genDistributedH(Random::genUniformReal_i0_e1(), 
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

	const real G = m_microfacet->shadowing(N, H, L, V);

	const real multiplier = std::abs(HoL / (NoV * NoL * NoH));
	out_pdfAppliedBsdf->set(F.mul(G).mulLocal(multiplier));

	*out_type = ESurfacePhenomenon::REFLECTION;
}

void OpaqueMicrofacet::calcSampleDirPdfW(const Intersection& X, const Vector3R& L, const Vector3R& V, const ESurfacePhenomenon& type,
                                         real* const out_pdfW) const
{
	const Vector3R& N = X.getHitSmoothNormal();

	const real NoL = N.dot(L);

	// H is on the hemisphere of N
	Vector3R H = L.add(V).normalizeLocal();
	if(NoL < 0.0_r)
	{
		H.mulLocal(-1.0_r);
	}

	const real NoH = N.dot(H);
	const real HoL = H.dot(L);
	const real D = m_microfacet->distribution(N, H);

	*out_pdfW = std::abs(D * NoH / (4.0_r * HoL));
}

}// end namespace ph