#include "Model/Material/PerfectMirrorSurfaceIntegrand.h"
#include "Core/Ray.h"
#include "Math/Vector3f.h"
#include "Math/random_number.h"
#include "Math/constant.h"
#include "Core/Intersection.h"

#include <cmath>
#include <iostream>
#include <limits>

#define DIRAC_DELTA_HEIGHT_APPROXIMATION 1000.0f

namespace ph
{

PerfectMirrorSurfaceIntegrand::~PerfectMirrorSurfaceIntegrand() = default;

void PerfectMirrorSurfaceIntegrand::genUniformRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const
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

void PerfectMirrorSurfaceIntegrand::genImportanceRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const
{
	*out_V = L.reflect(intersection.getHitNormal());
}

void PerfectMirrorSurfaceIntegrand::evaluateUniformRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const
{
	out_PDF->set(1.0f / (2.0f * PI_FLOAT32));
}

void PerfectMirrorSurfaceIntegrand::evaluateImportanceRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const
{
	out_PDF->set(DIRAC_DELTA_HEIGHT_APPROXIMATION);
}

void PerfectMirrorSurfaceIntegrand::evaluateLiWeight(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_LiWeight) const
{
	Vector3f reflectionDir = L.reflect(intersection.getHitNormal());

	if(reflectionDir.equals(V))
	{
		out_LiWeight->set(DIRAC_DELTA_HEIGHT_APPROXIMATION);
	}
	else
	{
		out_LiWeight->set(0, 0, 0);
	}
}

}// end namespace ph