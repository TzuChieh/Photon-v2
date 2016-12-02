#include "Model/Material/Integrand/SiOpaqueMicrofacet.h"
#include "Core/Ray.h"
#include "Math/Vector3f.h"
#include "Math/random_number.h"
#include "Math/constant.h"
#include "Core/Intersection.h"
#include "Model/Material/AbradedOpaque.h"

#include <cmath>

namespace ph
{

SiOpaqueMicrofacet::SiOpaqueMicrofacet() :
	m_albedo   (std::make_shared<ConstantTexture>(Vector3f(0.5f, 0.5f, 0.5f))), 
	m_roughness(std::make_shared<ConstantTexture>(Vector3f(0.5f, 0.5f, 0.5f))), 
	m_F0       (std::make_shared<ConstantTexture>(Vector3f(0.04f, 0.04f, 0.04f)))
{

}

SiOpaqueMicrofacet::~SiOpaqueMicrofacet() = default;

void SiOpaqueMicrofacet::genUniformRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const
{
	const float32 phi = 2.0f * PI_FLOAT32 * genRandomFloat32_0_1_uniform();
	const float32 yValue = genRandomFloat32_0_1_uniform();
	const float32 yRadius = sqrt(1.0f - yValue * yValue);

	out_V->x = cos(phi) * yRadius;
	out_V->y = yValue;
	out_V->z = sin(phi) * yRadius;

	Vector3f u;
	Vector3f v(intersection.getHitNormal());
	Vector3f w;

	v.calcOrthBasisAsYaxis(&u, &w);

	*out_V = u.mulLocal(out_V->x).
	addLocal(v.mulLocal(out_V->y)).
	addLocal(w.mulLocal(out_V->z));

	out_V->normalizeLocal();
}

void SiOpaqueMicrofacet::genImportanceRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const
{
	// for GGX (Trowbridge-Reitz) Normal Distribution Function

	Vector3f roughness;
	m_roughness->sample(intersection.getHitUVW(), &roughness);

	const float32 phi       = 2.0f * PI_FLOAT32 * genRandomFloat32_0_1_uniform();
	const float32 randNum   = genRandomFloat32_0_1_uniform();
	const float32 theta     = atan(roughness.x * roughness.x * sqrt(randNum / (1.0f - randNum)));

	const float32 sinTheta = sin(theta);
	const float32 cosTheta = cos(theta);

	Vector3f H;

	H.x = cos(phi) * sinTheta;
	H.y = cosTheta;
	H.z = sin(phi) * sinTheta;

	Vector3f u;
	Vector3f v(intersection.getHitNormal());
	Vector3f w;

	v.calcOrthBasisAsYaxis(&u, &w);

	H = u.mulLocal(H.x).addLocal(v.mulLocal(H.y)).addLocal(w.mulLocal(H.z));
	H.normalizeLocal();

	*out_V = L.mul(-1.0f).reflect(H).normalizeLocal();
}

void SiOpaqueMicrofacet::evaluateUniformRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const
{
	out_PDF->set(1.0f / (2.0f * PI_FLOAT32));
}

void SiOpaqueMicrofacet::evaluateImportanceRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const
{
	const Vector3f N = intersection.getHitNormal();
	const Vector3f H = V.add(L).normalizeLocal();

	const float32 D = calcNormalDistributionTerm(intersection, H);

	out_PDF->set(D * N.dot(H) / (4.0f * H.dot(L)));
}

void SiOpaqueMicrofacet::evaluateLiWeight(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_LiWeight) const
{
	const Vector3f& N = intersection.getHitNormal();
	const Vector3f& H = V.add(L).normalizeLocal();

	const float32   D = calcNormalDistributionTerm(intersection, H);
	const float32   G = calcGeometricShadowingTerm(intersection, L, V, H);
	const Vector3f& F = calcFresnelTerm(intersection, V, H);

	// notice that the (N dot L) term canceled out with the lambertian term
	out_LiWeight->set(F.mul(D * G).divLocal(4.0f * N.dot(V)));
}

float32 SiOpaqueMicrofacet::calcNormalDistributionTerm(const Intersection& intersection, const Vector3f& H) const
{
	// GGX (Trowbridge-Reitz) Normal Distribution Function

	const Vector3f& N = intersection.getHitNormal();

	const float32 NoH = N.dot(H);

	if(NoH <= 0.0f)
	{
		return 0.0f;
	}

	Vector3f roughness;
	m_roughness->sample(intersection.getHitUVW(), &roughness);

	const float32 alpha  = roughness.x * roughness.x;
	const float32 alpha2 = alpha * alpha;
	const float32 NoH2   = NoH * NoH;

	const float32 innerTerm = NoH2 * (alpha2 - 1.0f) + 1.0f;
	const float32 denominator = PI_FLOAT32 * innerTerm * innerTerm;

	return alpha2 / denominator;
}

//float32 SiOpaqueMicrofacet::calcGeometricShadowingTerm(const Vector3f& L, const Vector3f& V, const Vector3f& N, const Vector3f& H) const
//{
//	// Cook-Torrance Geometric Shadowing Function
//
//	const float32 VoH = V.dot(H);
//	const float32 NoH = N.dot(H);
//	const float32 NoV = N.dot(V);
//	const float32 NoL = N.dot(L);
//
//	const float32 termA = 2.0f * NoH * NoV / VoH;
//	const float32 termB = 2.0f * NoH * NoL / VoH;
//
//	return fmin(1.0f, fmin(termA, termB));
//}

float32 SiOpaqueMicrofacet::calcGeometricShadowingTerm(const Intersection& intersection, const Vector3f& L, const Vector3f& V, const Vector3f& H) const
{
	// Smith's GGX Geometry Shadowing Function

	const Vector3f& N = intersection.getHitNormal();

	const float32 HoV = H.dot(V);
	const float32 HoL = H.dot(L);
	const float32 NoV = N.dot(V);
	const float32 NoL = N.dot(L);

	if(HoL / NoL <= 0.0f || HoV / NoL <= 0.0f)
	{
		return 0.0f;
	}

	Vector3f roughness;
	m_roughness->sample(intersection.getHitUVW(), &roughness);

	const float32 alpha  = roughness.x * roughness.x;
	const float32 alpha2 = alpha * alpha;

	const float32 lightG = (2.0f * NoL) / (NoL + sqrt(alpha2 + (1.0f - alpha2) * NoL * NoL));
	const float32 viewG  = (2.0f * NoV) / (NoV + sqrt(alpha2 + (1.0f - alpha2) * NoV * NoV));

	return lightG * viewG;
}

Vector3f SiOpaqueMicrofacet::calcFresnelTerm(const Intersection& intersection, const Vector3f& V, const Vector3f& H) const
{
	// Schlick Approximated Fresnel Function

	Vector3f F0;
	m_F0->sample(intersection.getHitUVW(), &F0);
	const float32 VoH = V.dot(H);

	return F0.add(F0.complement().mulLocal(pow(1.0f - VoH, 5)));
}

}// end namespace ph