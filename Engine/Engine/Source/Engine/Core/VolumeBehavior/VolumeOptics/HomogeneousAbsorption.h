#pragma once

#include "Engine/Core/VolumeBehavior/VolumeOptics.h"
#include "Engine/Math/Color/spectrum_fwd.h"

#include <memory>

namespace ph
{

class MediumCoefficient;

class HomogeneousAbsorption : public VolumeOptics
{
public:
	HomogeneousAbsorption();
	explicit HomogeneousAbsorption(const math::Spectrum& sigmaA);

private:
	void genDistanceSample(
		const SurfaceHit& X,
		const math::Vector3R& L,
		real maxDist,
		real* out_dist,
		math::Spectrum* out_pdfAppliedWeight) const override;

	math::Spectrum m_sigmaA;
};

}// end namespace ph
