#include "Core/SurfaceBehavior/OpaqueMicrofacet.h"
#include "Core/Ray.h"
#include "Math/Vector3f.h"
#include "Math/random_number.h"
#include "Math/constant.h"
#include "Core/Intersection.h"
#include "Core/SurfaceBehavior/random_sample.h"
#include "Core/SurfaceBehavior/Microfacet.h"
#include "Core/SurfaceBehavior/SurfaceSample.h"

#include <cmath>
#include <iostream>

namespace ph
{

OpaqueMicrofacet::OpaqueMicrofacet() :
	m_albedo   (std::make_shared<ConstantTexture>(Vector3f(0.5f, 0.5f, 0.5f))), 
	m_roughness(std::make_shared<ConstantTexture>(Vector3f(0.5f, 0.5f, 0.5f))), 
	m_F0       (std::make_shared<ConstantTexture>(Vector3f(0.04f, 0.04f, 0.04f)))
{

}

OpaqueMicrofacet::~OpaqueMicrofacet() = default;

void OpaqueMicrofacet::genImportanceSample(SurfaceSample& sample) const
{
	// Cook-Torrance microfacet specular BRDF is D(H)*F(V, H)*G(L, V, H) / (4*NoL*NoV).
	// The importance sampling strategy is to generate a microfacet normal (H) which follows D(H)'s distribution, and
	// generate L by reflecting -V using H.
	// The PDF for this sampling scheme is D(H)*NoH / (4*HoL). The reason that (4*HoL) exists is because there's a 
	// jacobian involved (from H's probability space to L's).

	Vector3f sampledRoughness;
	m_roughness->sample(sample.X->getHitUVW(), &sampledRoughness);
	const float32 roughness = sampledRoughness.x;

	Vector3f sampledF0;
	m_F0->sample(sample.X->getHitUVW(), &sampledF0);

	const Vector3f& N = sample.X->getHitSmoothNormal();
	Vector3f H;

	genUnitHemisphereGgxTrowbridgeReitzNdfSample(genRandomFloat32_0_1_uniform(), genRandomFloat32_0_1_uniform(), roughness, &H);
	Vector3f u;
	Vector3f v(N);
	Vector3f w;
	v.calcOrthBasisAsYaxis(&u, &w);
	H = u.mulLocal(H.x).addLocal(v.mulLocal(H.y)).addLocal(w.mulLocal(H.z));
	H.normalizeLocal();

	const Vector3f V = sample.V;
	const Vector3f L = sample.V.mul(-1.0f).reflect(H).normalizeLocal();
	sample.L = L;

	const float32 NoV = N.dot(V);
	const float32 NoL = N.dot(L);
	const float32 HoV = H.dot(V);
	const float32 HoL = H.dot(L);
	const float32 NoH = N.dot(H);

	// sidedness agreement between real geometry and shading (phong-interpolated) normal
	if(NoV * sample.X->getHitGeoNormal().dot(V) <= 0.0f || NoL * sample.X->getHitGeoNormal().dot(L) <= 0.0f)
	{
		sample.liWeight.set(0, 0, 0);
		return;
	}

	Vector3f F;
	const float32 G = Microfacet::geometryShadowingGgxSmith(NoV, NoL, HoV, HoL, roughness);
	Microfacet::fresnelSchlickApproximated(HoV, sampledF0, &F);

	/*const float32   G = calcGeometricShadowingTerm(intersection, L, V, H);
	const Vector3f& F = calcFresnelTerm(intersection, V, H);*/

	// notice that the (N dot L) term canceled out with the lambertian term
	sample.liWeight.set(F.mul(G * HoL).divLocal(NoV * NoH));

	// this model reflects light
	sample.type = ESurfaceSampleType::REFLECTION;
}

void OpaqueMicrofacet::evaluate(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_value) const
{
	const Vector3f& N = intersection.getHitSmoothNormal();

	const float32 NoL = N.dot(L);
	const float32 NoV = N.dot(V);

	// check if L, V lies on different side of the surface
	if(NoL * NoV <= 0.0f)
	{
		out_value->set(0, 0, 0);
		return;
	}

	// sidedness agreement between real geometry and shading (phong-interpolated) normal
	if(NoL * intersection.getHitGeoNormal().dot(L) <= 0.0f || NoV * intersection.getHitGeoNormal().dot(V) <= 0.0f)
	{
		out_value->set(0, 0, 0);
		return;
	}

	Vector3f sampledRoughness;
	m_roughness->sample(intersection.getHitUVW(), &sampledRoughness);
	const float32 roughness = sampledRoughness.x;
	Vector3f sampledF0;
	m_F0->sample(intersection.getHitUVW(), &sampledF0);

	// H is on the hemisphere of N
	Vector3f H = L.add(V).normalizeLocal();
	if(NoL < 0.0f)
	{
		H.mulLocal(-1.0f);
	}

	const float32 HoV = H.dot(V);
	const float32 NoH = N.dot(H);
	const float32 HoL = H.dot(L);

	Vector3f F;
	Microfacet::fresnelSchlickApproximated(std::abs(HoV), sampledF0, &F);
	const float32 D = Microfacet::normalDistributionGgxTrowbridgeReitz(NoH, roughness);
	const float32 G = Microfacet::geometryShadowingGgxSmith(NoV, NoL, HoV, HoL, roughness);

	// notice that the abs(N dot L) term canceled out with the lambertian term
	*out_value = F.mul(D * G / (4.0f * std::abs(NoV)));
}

}// end namespace ph