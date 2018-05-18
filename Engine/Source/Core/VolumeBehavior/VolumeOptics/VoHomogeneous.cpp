#include "Core/VolumeBehavior/VolumeOptics/VoHomogeneous.h"

namespace ph
{

VoHomogeneous::~VoHomogeneous() = default;

void VoHomogeneous::sampleDistance(
	const Vector3R& X,
	const Vector3R& V,
	const real maxDist,
	real* const out_dist,
	SpectralStrength* const out_pdfAppliedWeight) const
{
	// TODO
}

}// end namespace ph