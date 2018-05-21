#include "Core/VolumeBehavior/VolumeOptics/VoHomoAbsorption.h"
#include "Core/VolumeBehavior/BlockFunction/BfConstant.h"
#include "Common/assertion.h"
#include "Math/Random.h"

#include <cmath>

namespace ph
{

VoHomoAbsorption::VoHomoAbsorption() : 
	VoHomoAbsorption(std::make_unique<BfConstant>())
{}

VoHomoAbsorption::VoHomoAbsorption(const std::shared_ptr<BfConstant>& blockFunc) : 
	VolumeOptics(),
	m_blockFunc(blockFunc)
{
	PH_ASSERT(blockFunc);
}

VoHomoAbsorption::~VoHomoAbsorption() = default;

void VoHomoAbsorption::sampleDistance(
	const SurfaceHit& X,
	const Vector3R& V,
	const real maxDist,
	real* const out_dist,
	SpectralStrength* const out_pdfAppliedWeight) const
{
	PH_ASSERT(m_blockFunc && out_dist && out_pdfAppliedWeight);

	*out_dist = maxDist;

	// PDF = 1
	*out_pdfAppliedWeight = m_blockFunc->calcTransmittance(maxDist);
}

}// end namespace ph