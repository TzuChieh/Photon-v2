#include "Core/SurfaceBehavior/BSDF/OpaqueMicrofacet.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"
#include "Math/Random.h"
#include "Math/constant.h"
#include "Core/Intersection.h"
#include "Core/SurfaceBehavior/BSDF/random_sample.h"
#include "Core/SurfaceBehavior/BSDF/Microfacet.h"
#include "Core/Sample/SurfaceSample.h"
#include "Math/Math.h"

#include <cmath>
#include <iostream>

namespace ph
{

OpaqueMicrofacet::OpaqueMicrofacet() :
	m_albedo(std::make_shared<ConstantTexture>(Vector3R(0.5_r,  0.5_r,  0.5_r))),
	m_alpha (std::make_shared<ConstantTexture>(Vector3R(0.5_r,  0.5_r,  0.5_r))),
	m_F0    (std::make_shared<ConstantTexture>(Vector3R(0.04_r, 0.04_r, 0.04_r)))
{

}

OpaqueMicrofacet::~OpaqueMicrofacet() = default;

void OpaqueMicrofacet::genImportanceSample(SurfaceSample& sample) const
{
	ESurfacePhenomenon type;
	genSample(*sample.X, sample.V, &sample.L, &sample.liWeight, &type);
	sample.type = ESurfaceSampleType::REFLECTION;
}

real OpaqueMicrofacet::calcImportanceSamplePdfW(const SurfaceSample& sample) const
{
	real pdfW;
	calcSampleDirPdfW(*sample.X, sample.L, sample.V, ESurfacePhenomenon::REFLECTION, &pdfW);
	return pdfW;
}

void OpaqueMicrofacet::evaluate(SurfaceSample& sample) const
{
	ESurfacePhenomenon type;
	evaluate(*sample.X, sample.L, sample.V, &sample.liWeight, &type);
	sample.type = ESurfaceSampleType::REFLECTION;
}

void OpaqueMicrofacet::evaluate(const Intersection& X, const Vector3R& L, const Vector3R& V,
                                Vector3R* const out_bsdf, ESurfacePhenomenon* const out_type) const
{
	const Vector3R& N = X.getHitSmoothNormal();

	const real NoL = N.dot(L);
	const real NoV = N.dot(V);

	// check if L, V lies on different side of the surface
	if(NoL * NoV <= 0.0_r)
	{
		out_bsdf->set(0, 0, 0);
		return;
	}

	Vector3R sampledAlpha;
	m_alpha->sample(X.getHitUVW(), &sampledAlpha);
	const real alpha = sampledAlpha.x;
	Vector3R sampledF0;
	m_F0->sample(X.getHitUVW(), &sampledF0);

	// H is on the hemisphere of N
	Vector3R H = L.add(V).normalizeLocal();
	if(NoL < 0.0_r)
	{
		H.mulLocal(-1.0_r);
	}

	const real HoV = H.dot(V);
	const real NoH = N.dot(H);
	const real HoL = H.dot(L);

	Vector3R F;
	Microfacet::fresnelSchlickApproximated(std::abs(HoV), sampledF0, &F);
	const real D = Microfacet::normalDistributionGgxTrowbridgeReitz(NoH, alpha);
	const real G = Microfacet::geometryShadowingGgxSmith(NoV, NoL, HoV, HoL, alpha);

	// notice that the abs(N dot L) term canceled out with the lambertian term
	*out_bsdf = F.mul(D * G / (4.0_r * std::abs(NoV)));
	
	*out_type = ESurfacePhenomenon::REFLECTION;
}

void OpaqueMicrofacet::genSample(const Intersection& X, const Vector3R& V,
                                 Vector3R* const out_L, Vector3R* const out_pdfAppliedBsdf, ESurfacePhenomenon* const out_type) const
{
	// Cook-Torrance microfacet specular BRDF is D(H)*F(V, H)*G(L, V, H) / (4*NoL*NoV).
	// The importance sampling strategy is to generate a microfacet normal (H) which follows D(H)'s distribution, and
	// generate L by reflecting -V using H.
	// The PDF for this sampling scheme is D(H)*NoH / |4*HoL|. The reason that |4*HoL| exists is because there's a 
	// jacobian involved (from H's probability space to L's).

	Vector3R sampledAlpha;
	m_alpha->sample(X.getHitUVW(), &sampledAlpha);
	const real alpha = sampledAlpha.x;

	Vector3R sampledF0;
	m_F0->sample(X.getHitUVW(), &sampledF0);

	const Vector3R& N = X.getHitSmoothNormal();
	Vector3R H;

	genUnitHemisphereGgxTrowbridgeReitzNdfSample(Random::genUniformReal_i0_e1(), Random::genUniformReal_i0_e1(), alpha, &H);
	Vector3R u;
	Vector3R v(N);
	Vector3R w;
	Math::formOrthonormalBasis(v, &u, &w);
	H = u.mulLocal(H.x).addLocal(v.mulLocal(H.y)).addLocal(w.mulLocal(H.z));
	H.normalizeLocal();

	const Vector3R L = V.mul(-1.0_r).reflect(H).normalizeLocal();
	*out_L = L;

	const real NoV = N.dot(V);
	const real NoL = N.dot(L);
	const real HoV = H.dot(V);
	const real HoL = H.dot(L);
	const real NoH = N.dot(H);

	Vector3R F;
	const real G = Microfacet::geometryShadowingGgxSmith(NoV, NoL, HoV, HoL, alpha);
	Microfacet::fresnelSchlickApproximated(std::abs(HoV), sampledF0, &F);

	// notice that the (N dot L) term canceled out with the lambertian term
	out_pdfAppliedBsdf->set(F.mul(G * HoL).divLocal(NoV * NoH));

	*out_type = ESurfacePhenomenon::REFLECTION;
}

void OpaqueMicrofacet::calcSampleDirPdfW(const Intersection& X, const Vector3R& L, const Vector3R& V, const ESurfacePhenomenon& type,
                                         real* const out_pdfW) const
{
	Vector3R sampledAlpha;
	m_alpha->sample(X.getHitUVW(), &sampledAlpha);
	const real alpha = sampledAlpha.x;

	const real NoL = X.getHitSmoothNormal().dot(L);

	// H is on the hemisphere of N
	Vector3R H = L.add(V).normalizeLocal();
	if(NoL < 0.0_r)
	{
		H.mulLocal(-1.0_r);
	}

	const real NoH = X.getHitSmoothNormal().dot(H);
	const real HoL = H.dot(L);
	const real D = Microfacet::normalDistributionGgxTrowbridgeReitz(NoH, alpha);

	*out_pdfW = std::abs(D * NoH / (4.0_r * HoL));
}

}// end namespace ph