#pragma once

#include "Math/math_fwd.h"
#include "Math/Color/Spectrum.h"

namespace ph
{

class Scene;
class SurfaceHit;

class PtVolumetricEstimator final
{
public:
	static void sample(
		const Scene& scene,
		const SurfaceHit& Xs,
		const math::Vector3R& L,
		SurfaceHit* out_Xe,
		math::Vector3R* out_V,
		math::Spectrum* out_weight,
		math::Spectrum* out_radiance);
};

}// end namespace ph
