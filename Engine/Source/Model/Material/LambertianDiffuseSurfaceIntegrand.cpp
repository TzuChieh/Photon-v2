#include "Model/Material/LambertianDiffuseSurfaceIntegrand.h"
#include "Core/Ray.h"
#include "Math/Vector3f.h"

namespace ph
{

LambertianDiffuseSurfaceIntegrand::~LambertianDiffuseSurfaceIntegrand() = default;

bool LambertianDiffuseSurfaceIntegrand::sampleLiWeight(const Vector3f& L, const Vector3f& V, const Vector3f& N, Ray& ray) const
{
	// HACK
	ray.accumulateLiWeight(Vector3f(N.dot(L)));

	return true;
}

}// end namespace ph