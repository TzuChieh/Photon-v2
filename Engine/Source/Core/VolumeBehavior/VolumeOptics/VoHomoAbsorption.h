#pragma once

#include "Core/VolumeBehavior/VolumeOptics.h"

#include <memory>

namespace ph
{

class BfConstant;

class VoHomoAbsorption final : public VolumeOptics
{
public:
	VoHomoAbsorption();
	VoHomoAbsorption(const std::shared_ptr<BfConstant>& blockFunc);
	virtual ~VoHomoAbsorption() override;

private:
	virtual void sampleDistance(
		const SurfaceHit& X,
		const math::Vector3R& L,
		real maxDist,
		real* out_dist,
		SpectralStrength* out_pdfAppliedWeight) const override;

	std::shared_ptr<BfConstant> m_blockFunc;
};

}// end namespace ph
