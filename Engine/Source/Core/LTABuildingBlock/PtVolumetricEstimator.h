#pragma once

#include "Math/math_fwd.h"
#include "Core/Quantity/SpectralStrength.h"

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
		SpectralStrength* out_weight,
		SpectralStrength* out_radiance);
};

}// end namespace ph
