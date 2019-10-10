#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/LTABuildingBlock/SidednessAgreement.h"

namespace ph
{

class Scene;
class SurfaceHit;
class Time;

template<ESaPolicy POLICY>
class TDirectLightEstimator
{
public:
	explicit TDirectLightEstimator(const Scene* scene);

	bool sample(
		const SurfaceHit& X,
		const Time&       time,
		math::Vector3R*   out_L,
		real*             out_pdfW,
		SpectralStrength* out_emittedRadiance);

	real samplePdfWUnoccluded(
		const SurfaceHit& X,
		const SurfaceHit& Xe,
		const Time&       time);

private:
	const Scene* m_scene;
};

}// end namespace ph

#include "Core/LTABuildingBlock/TDirectLightEstimator.ipp"
