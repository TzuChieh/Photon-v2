#include "Core/SurfaceBehavior/OpaqueMicrofacet.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"
#include "Math/random_number.h"
#include "Math/constant.h"
#include "Core/Intersection.h"
#include "Core/SurfaceBehavior/random_sample.h"
#include "Core/SurfaceBehavior/Microfacet.h"
#include "Core/Sample/SurfaceSample.h"

#include <cmath>
#include <iostream>

namespace ph
{

OpaqueMicrofacet::OpaqueMicrofacet() :
	m_albedo(std::make_shared<ConstantTexture>(Vector3R(0.5f, 0.5f, 0.5f))),
	m_alpha (std::make_shared<ConstantTexture>(Vector3R(0.5f, 0.5f, 0.5f))),
	m_F0    (std::make_shared<ConstantTexture>(Vector3R(0.04f, 0.04f, 0.04f)))
{

}

OpaqueMicrofacet::~OpaqueMicrofacet() = default;

void OpaqueMicrofacet::genImportanceSample(SurfaceSample& sample) const
{
	// Cook-Torrance microfacet specular BRDF is D(H)*F(V, H)*G(L, V, H) / (4*NoL*NoV).
	// The importance sampling strategy is to generate a microfacet normal (H) which follows D(H)'s distribution, and
	// generate L by reflecting -V using H.
	// The PDF for this sampling scheme is D(H)*NoH / |4*HoL|. The reason that |4*HoL| exists is because there's a 
	// jacobian involved (from H's probability space to L's).

	Vector3R sampledAlpha;
	m_alpha->sample(sample.X->getHitUVW(), &sampledAlpha);
	const float32 alpha = sampledAlpha.x;

	Vector3R sampledF0;
	m_F0->sample(sample.X->getHitUVW(), &sampledF0);

	const Vector3R& N = sample.X->getHitSmoothNormal();
	Vector3R H;

	genUnitHemisphereGgxTrowbridgeReitzNdfSample(genRandomFloat32_0_1_uniform(), genRandomFloat32_0_1_uniform(), alpha, &H);
	Vector3R u;
	Vector3R v(N);
	Vector3R w;
	v.calcOrthBasisAsYaxis(&u, &w);
	H = u.mulLocal(H.x).addLocal(v.mulLocal(H.y)).addLocal(w.mulLocal(H.z));
	H.normalizeLocal();

	const Vector3R V = sample.V;
	const Vector3R L = sample.V.mul(-1.0f).reflect(H).normalizeLocal();
	sample.L = L;

	const float32 NoV = N.dot(V);
	const float32 NoL = N.dot(L);
	const float32 HoV = H.dot(V);
	const float32 HoL = H.dot(L);
	const float32 NoH = N.dot(H);

	Vector3R F;
	const float32 G = Microfacet::geometryShadowingGgxSmith(NoV, NoL, HoV, HoL, alpha);
	Microfacet::fresnelSchlickApproximated(std::abs(HoV), sampledF0, &F);

	// notice that the (N dot L) term canceled out with the lambertian term
	sample.liWeight.set(F.mul(G * HoL).divLocal(NoV * NoH));

	// this model reflects light
	sample.type = ESurfaceSampleType::REFLECTION;
}

float32 OpaqueMicrofacet::calcImportanceSamplePdfW(const SurfaceSample& sample) const
{
	Vector3R sampledAlpha;
	m_alpha->sample(sample.X->getHitUVW(), &sampledAlpha);
	const float32 alpha = sampledAlpha.x;

	const float32 NoL = sample.X->getHitSmoothNormal().dot(sample.L);

	// H is on the hemisphere of N
	Vector3R H = sample.L.add(sample.V).normalizeLocal();
	if(NoL < 0.0f)
	{
		H.mulLocal(-1.0f);
	}

	const float32 NoH = sample.X->getHitSmoothNormal().dot(H);
	const float32 HoL = H.dot(sample.L);
	const float32 D = Microfacet::normalDistributionGgxTrowbridgeReitz(NoH, alpha);

	return std::abs(D * NoH / (4.0f * HoL));
}

void OpaqueMicrofacet::evaluate(SurfaceSample& sample) const
{
	const Vector3R& N = sample.X->getHitSmoothNormal();

	const float32 NoL = N.dot(sample.L);
	const float32 NoV = N.dot(sample.V);

	// check if L, V lies on different side of the surface
	if(NoL * NoV <= 0.0f)
	{
		sample.liWeight.set(0, 0, 0);
		return;
	}

	Vector3R sampledAlpha;
	m_alpha->sample(sample.X->getHitUVW(), &sampledAlpha);
	const float32 alpha = sampledAlpha.x;
	Vector3R sampledF0;
	m_F0->sample(sample.X->getHitUVW(), &sampledF0);

	// H is on the hemisphere of N
	Vector3R H = sample.L.add(sample.V).normalizeLocal();
	if(NoL < 0.0f)
	{
		H.mulLocal(-1.0f);
	}

	const float32 HoV = H.dot(sample.V);
	const float32 NoH = N.dot(H);
	const float32 HoL = H.dot(sample.L);

	Vector3R F;
	Microfacet::fresnelSchlickApproximated(std::abs(HoV), sampledF0, &F);
	const float32 D = Microfacet::normalDistributionGgxTrowbridgeReitz(NoH, alpha);
	const float32 G = Microfacet::geometryShadowingGgxSmith(NoV, NoL, HoV, HoL, alpha);

	// notice that the abs(N dot L) term canceled out with the lambertian term
	sample.liWeight = F.mul(D * G / (4.0f * std::abs(NoV)));
	sample.type = ESurfaceSampleType::REFLECTION;
}

}// end namespace ph