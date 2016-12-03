#include "Model/Material/Integrand/SiTranslucentMicrofacet.h"
#include "Image/ConstantTexture.h"
#include "Core/Ray.h"
#include "Math/Vector3f.h"
#include "Math/random_number.h"
#include "Math/constant.h"
#include "Core/Intersection.h"
#include "Model/Material/Integrand/random_sample.h"

#include <memory>

namespace ph
{

SiTranslucentMicrofacet::SiTranslucentMicrofacet() :
	m_F0       (std::make_shared<ConstantTexture>(Vector3f(0.04f, 0.04f, 0.04f))),
	m_IOR      (std::make_shared<ConstantTexture>(Vector3f(1.0f, 1.0f, 1.0f))),
	m_roughness(std::make_shared<ConstantTexture>(Vector3f(0.5f, 0.5f, 0.5f)))
{

}

SiTranslucentMicrofacet::~SiTranslucentMicrofacet() = default;

void SiTranslucentMicrofacet::evaluateImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const
{
	// Cook-Torrance microfacet specular BRDF is D(H)*F(V, H)*G(L, V, H) / (4*NoL*NoV).
	// The importance sampling strategy is to generate a microfacet normal (H) which follows D(H)'s distribution, and
	// generate L by reflecting/refracting -V using H.
	// The PDF for this sampling scheme is D(H)*NoH / (4*HoL). The reason that (4*HoL) exists is because there's a 
	// jacobian involved (from H's probability space to L's).

	const Vector3f V = ray.getDirection().mul(-1.0f);
	const Vector3f& N = intersection.getHitNormal();
	Vector3f H;
	Vector3f roughness;
	m_roughness->sample(intersection.getHitUVW(), &roughness);

	genUnitHemisphereGgxTrowbridgeReitzNdfSample(genRandomFloat32_0_1_uniform(), genRandomFloat32_0_1_uniform(), roughness.x, &H);
	Vector3f u;
	Vector3f v(N);
	Vector3f w;
	v.calcOrthBasisAsYaxis(&u, &w);
	H = u.mulLocal(H.x).addLocal(v.mulLocal(H.y)).addLocal(w.mulLocal(H.z));
	H.normalizeLocal();

	Vector3f F = calcFresnelTerm(intersection, V, H);

	// use Fresnel term to select which path to take and calculate L

	Vector3f L;
	const float32 dart = genRandomFloat32_0_1_uniform();
	const float32 reflectProb = F.avg();

	// reflect path
	if(dart < reflectProb)
	{
		// calculate L
		L = ray.getDirection().reflect(H).normalizeLocal();

		// account for probability
		F.divLocal(reflectProb);

		// this path reflects light
		out_sample->m_type = ESurfaceSampleType::REFLECTION;
	}
	// refract path
	else
	{
		float32 signNoV = N.dot(V) < 0.0f ? -1.0f : 1.0f;
		Vector3f ior;
		m_IOR->sample(intersection.getHitUVW(), &ior);

		// assume the outside medium has an IOR of 1.0 (which is true in most cases)
		const float32 iorRatio = signNoV < 0.0f ? ior.x : 1.0f / ior.x;
		const float32 sqrValue = 1.0f - iorRatio*iorRatio*(1.0f - V.dot(H)*V.dot(H));

		Vector3f T;

		// TIR (total internal reflection)
		if(sqrValue < 0.0f)
		{
			T = V.mul(-1.0f).reflectLocal(H).normalizeLocal();

			// this path reflects light
			out_sample->m_type = ESurfaceSampleType::REFLECTION;
		}
		// refraction
		else
		{
			const float32 Hfactor = iorRatio*V.dot(H) - signNoV * sqrt(sqrValue);
			const float32 Vfactor = -iorRatio;
			T = H.mul(Hfactor).addLocal(V.mul(Vfactor)).normalizeLocal();

			// this path refracts light
			out_sample->m_type = ESurfaceSampleType::REFLECTION;
		}

		// calculate L
		L = T;

		// account for probability
		F = F.complement().divLocal(1.0f - reflectProb);
	}

	out_sample->m_direction = L;

	const float32 G = calcGeometricShadowingTerm(intersection, L, V, H);

	// notice that the (N dot L) term canceled out with the lambertian term
	out_sample->m_LiWeight.set(F.mul(G * H.absDot(L)).divLocal(N.absDot(V) * N.absDot(H)));
}

float32 SiTranslucentMicrofacet::calcNormalDistributionTerm(const Intersection& intersection, const Vector3f& H) const
{
	// GGX (Trowbridge-Reitz) Normal Distribution Function

	const Vector3f& N = intersection.getHitNormal();

	const float32 NoH = N.absDot(H);

	if(NoH <= 0.0f)
	{
		return 0.0f;
	}

	Vector3f roughness;
	m_roughness->sample(intersection.getHitUVW(), &roughness);

	const float32 alpha = roughness.x * roughness.x;
	const float32 alpha2 = alpha * alpha;
	const float32 NoH2 = NoH * NoH;

	const float32 innerTerm = NoH2 * (alpha2 - 1.0f) + 1.0f;
	const float32 denominator = PI_FLOAT32 * innerTerm * innerTerm;

	return alpha2 / denominator;
}

float32 SiTranslucentMicrofacet::calcGeometricShadowingTerm(const Intersection& intersection, const Vector3f& L, const Vector3f& V, const Vector3f& H) const
{
	// Smith's GGX Geometry Shadowing Function

	const Vector3f& N = intersection.getHitNormal();

	const float32 HoV = H.absDot(V);
	const float32 HoL = H.absDot(L);
	const float32 NoV = N.absDot(V);
	const float32 NoL = N.absDot(L);

	if(HoL / NoL <= 0.0f || HoV / NoL <= 0.0f)
	{
		return 0.0f;
	}

	Vector3f roughness;
	m_roughness->sample(intersection.getHitUVW(), &roughness);

	const float32 alpha = roughness.x * roughness.x;
	const float32 alpha2 = alpha * alpha;

	const float32 lightG = (2.0f * NoL) / (NoL + sqrt(alpha2 + (1.0f - alpha2) * NoL * NoL));
	const float32 viewG = (2.0f * NoV) / (NoV + sqrt(alpha2 + (1.0f - alpha2) * NoV * NoV));

	return lightG * viewG;
}

Vector3f SiTranslucentMicrofacet::calcFresnelTerm(const Intersection& intersection, const Vector3f& V, const Vector3f& H) const
{
	// Schlick Approximated Fresnel Function

	Vector3f F0;
	m_F0->sample(intersection.getHitUVW(), &F0);
	const float32 VoH = V.absDot(H);

	return F0.add(F0.complement().mulLocal(pow(1.0f - VoH, 5)));
}

}// end namespace ph