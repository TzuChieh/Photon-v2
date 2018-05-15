#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Core/Quantity/SpectralStrength.h"

namespace ph
{

class Scene;
class SurfaceHit;
class Time;

class PtDirectLightEstimator final
{
public:
	static bool sampleDirectLighting(
		const Scene&      scene, 
		const SurfaceHit& targetPos,
		const Time&       time,
		Vector3R*         out_L,
		real*             out_pdfW,
		SpectralStrength* out_emittedRadiance);
};

}// end namespace ph
