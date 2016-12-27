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

namespace ph
{

OpaqueMicrofacet::OpaqueMicrofacet() :
	m_albedo   (std::make_shared<ConstantTexture>(Vector3f(0.5f, 0.5f, 0.5f))), 
	m_roughness(std::make_shared<ConstantTexture>(Vector3f(0.5f, 0.5f, 0.5f))), 
	m_F0       (std::make_shared<ConstantTexture>(Vector3f(0.04f, 0.04f, 0.04f)))
{

}

OpaqueMicrofacet::~OpaqueMicrofacet() = default;

void OpaqueMicrofacet::genImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const
{
	// Cook-Torrance microfacet specular BRDF is D(H)*F(V, H)*G(L, V, H) / (4*NoL*NoV).
	// The importance sampling strategy is to generate a microfacet normal (H) which follows D(H)'s distribution, and
	// generate L by reflecting -V using H.
	// The PDF for this sampling scheme is D(H)*NoH / (4*HoL). The reason that (4*HoL) exists is because there's a 
	// jacobian involved (from H's probability space to L's).

	Vector3f sampledRoughness;
	m_roughness->sample(intersection.getHitUVW(), &sampledRoughness);
	const float32 roughness = sampledRoughness.x;

	Vector3f sampledF0;
	m_F0->sample(intersection.getHitUVW(), &sampledF0);

	const Vector3f& N = intersection.getHitSmoothNormal();
	Vector3f H;

	genUnitHemisphereGgxTrowbridgeReitzNdfSample(genRandomFloat32_0_1_uniform(), genRandomFloat32_0_1_uniform(), roughness, &H);
	Vector3f u;
	Vector3f v(N);
	Vector3f w;
	v.calcOrthBasisAsYaxis(&u, &w);
	H = u.mulLocal(H.x).addLocal(v.mulLocal(H.y)).addLocal(w.mulLocal(H.z));
	H.normalizeLocal();

	const Vector3f V = ray.getDirection().mul(-1.0f);
	const Vector3f L = ray.getDirection().reflect(H).normalizeLocal();
	out_sample->m_direction = L;

	const float32 NoV = N.dot(V);
	const float32 NoL = N.dot(L);
	const float32 HoV = H.dot(V);
	const float32 HoL = H.dot(L);
	const float32 NoH = N.dot(H);

	// sidedness agreement between real geometry and shading (phong-interpolated) normal
	if(NoV * intersection.getHitGeoNormal().dot(V) <= 0.0f || NoL * intersection.getHitGeoNormal().dot(L) <= 0.0f)
	{
		out_sample->m_LiWeight.set(0, 0, 0);
		out_sample->m_direction.set(ray.getDirection().reflect(H).normalizeLocal());
		out_sample->m_type = ESurfaceSampleType::REFLECTION;
		out_sample->m_emittedRadiance.set(0, 0, 0);
		return;
	}

	Vector3f F;
	const float32 G = Microfacet::geometryShadowingGgxSmith(NoV, NoL, HoV, HoL, roughness);
	Microfacet::fresnelSchlickApproximated(HoV, sampledF0, &F);

	/*const float32   G = calcGeometricShadowingTerm(intersection, L, V, H);
	const Vector3f& F = calcFresnelTerm(intersection, V, H);*/

	// notice that the (N dot L) term canceled out with the lambertian term
	out_sample->m_LiWeight.set(F.mul(G * HoL).divLocal(NoV * NoH));

	// this model reflects light
	out_sample->m_type = ESurfaceSampleType::REFLECTION;
}

}// end namespace ph