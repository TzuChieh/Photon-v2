#pragma once

#include "Core/VolumeBehavior/VolumeOptics.h"

#include <memory>

namespace ph
{

class VoHomogeneousAbsorption final : public VolumeOptics
{
public:
	virtual ~VoHomogeneousAbsorption() override;

private:
	virtual void sampleDistance(
		const Vector3R& X,
		const Vector3R& V,
		real maxDist,
		real* out_dist,
		SpectralStrength* out_pdfAppliedWeight) const override;

	std::shared_ptr<BlockFunction> m_blockFunc;
};

}// end namespace ph