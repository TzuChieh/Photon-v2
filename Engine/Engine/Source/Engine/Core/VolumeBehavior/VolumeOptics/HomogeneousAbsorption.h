#pragma once

#include "Engine/Core/VolumeBehavior/VolumeOptics.h"

#include <memory>

namespace ph
{

class MediumCoefficient;

class HomogeneousAbsorption : public VolumeOptics
{
public:
	HomogeneousAbsorption();
	explicit HomogeneousAbsorption(const std::shared_ptr<MediumCoefficient>& sigmaA);

private:
	void genDistanceSample(
		const SurfaceHit& X,
		const math::Vector3R& L,
		real maxDist,
		real* out_dist,
		math::Spectrum* out_pdfAppliedWeight) const override;

	std::shared_ptr<MediumCoefficient> m_sigmaA;
};

}// end namespace ph
