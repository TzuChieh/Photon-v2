#pragma once

#include "Model/Material/Integrand/SiLight.h"
#include "Math/Vector3f.h"
#include "Core/Ray.h"
#include "Math/random_number.h"
#include "Math/constant.h"
#include "Core/Intersection.h"

#include <iostream>

namespace ph
{

SiLight::SiLight() :
	m_emittedRadiance(0, 0, 0)
{

}

SiLight::~SiLight() = default;

void SiLight::genUniformRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const
{
	const float32 rand1 = genRandomFloat32_0_1_uniform();
	const float32 rand2 = genRandomFloat32_0_1_uniform();

	const float32 phi = 2.0f * PI_FLOAT32 * rand1;
	const float32 yValue = rand2;
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

void SiLight::genImportanceRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const
{
	genUniformRandomV(intersection, L, out_V);
}

void SiLight::evaluateUniformRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const
{
	std::cerr << "LightSurfaceIntegrand::evaluateUniformRandomVPDF() not implemented" << std::endl;
}

void SiLight::evaluateImportanceRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const
{
	std::cerr << "LightSurfaceIntegrand::evaluateImportanceRandomVPDF() not implemented" << std::endl;
}

void SiLight::evaluateLiWeight(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_LiWeight) const
{
	std::cerr << "LightSurfaceIntegrand::evaluateLiWeight() not implemented" << std::endl;
}

void SiLight::evaluateImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const
{
	// This lighting model is simple: the surface emits constant amount of radiance to any direction on
	// a specific point.

	out_sample->m_emittedRadiance = m_emittedRadiance;
	out_sample->m_type = ESurfaceSampleType::EMISSION;
}

}// end namespace ph