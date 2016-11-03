#pragma once

#include "Model/Material/LightSurfaceIntegrand.h"
#include "Math/Vector3f.h"
#include "Core/Ray.h"
#include "Math/random_number.h"
#include "Math/constant.h"
#include "Core/Intersection.h"

#include <iostream>

namespace ph
{

LightSurfaceIntegrand::LightSurfaceIntegrand() : 
	m_emittedRadiance(0, 0, 0)
{

}

LightSurfaceIntegrand::~LightSurfaceIntegrand() = default;

void LightSurfaceIntegrand::genUniformRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const
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

void LightSurfaceIntegrand::genImportanceRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const
{
	genUniformRandomV(intersection, L, out_V);
}

void LightSurfaceIntegrand::evaluateUniformRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const
{
	std::cerr << "LightSurfaceIntegrand::evaluateUniformRandomVPDF() not implemented" << std::endl;
}

void LightSurfaceIntegrand::evaluateImportanceRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const
{
	std::cerr << "LightSurfaceIntegrand::evaluateImportanceRandomVPDF() not implemented" << std::endl;
}

void LightSurfaceIntegrand::evaluateLiWeight(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_LiWeight) const
{
	std::cerr << "LightSurfaceIntegrand::evaluateLiWeight() not implemented" << std::endl;
}

}// end namespace ph