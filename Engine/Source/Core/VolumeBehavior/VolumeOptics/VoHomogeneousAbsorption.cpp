#include "Core/VolumeBehavior/VolumeOptics/VoHomogeneousAbsorption.h"

namespace ph
{

VoHomogeneousAbsorption::~VoHomogeneousAbsorption() = default;

void VoHomogeneousAbsorption::sampleDistance(
	const Vector3R& X,
	const Vector3R& V,
	const real maxDist,
	real* const out_dist,
	SpectralStrength* const out_pdfAppliedWeight) const
{
	// TODO
}

}// end namespace ph