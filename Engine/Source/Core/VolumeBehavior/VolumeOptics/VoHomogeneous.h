#pragma once

#include "Core/VolumeBehavior/VolumeOptics.h"

namespace ph
{

class VoHomogeneous final : public VolumeOptics
{
public:
	virtual ~VoHomogeneous() override;

private:
	virtual void sampleDistance(
		const Vector3R& X,
		const Vector3R& V,
		real maxDist,
		real* out_dist,
		SpectralStrength* out_pdfAppliedWeight) const override;
};

}// end namespace ph