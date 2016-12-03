#include "Model/Material/Integrand/SiTranslucentMicrofacet.h"
#include "Image/ConstantTexture.h"
#include "Core/Ray.h"
#include "Math/Vector3f.h"
#include "Math/random_number.h"
#include "Math/constant.h"
#include "Core/Intersection.h"

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

void SiTranslucentMicrofacet::genUniformRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const
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

void SiTranslucentMicrofacet::genImportanceRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const
{

}

void SiTranslucentMicrofacet::evaluateUniformRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const
{
	out_PDF->set(1.0f / (2.0f * PI_FLOAT32));
}

void SiTranslucentMicrofacet::evaluateImportanceRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const
{

}

void SiTranslucentMicrofacet::evaluateLiWeight(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_LiWeight) const
{

}

void SiTranslucentMicrofacet::evaluateImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const
{

}

float32 SiTranslucentMicrofacet::calcNormalDistributionTerm(const Intersection& intersection, const Vector3f& H) const
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
	const float32 VoH = V.dot(H);

	return F0.add(F0.complement().mulLocal(pow(1.0f - VoH, 5)));
}

}// end namespace ph