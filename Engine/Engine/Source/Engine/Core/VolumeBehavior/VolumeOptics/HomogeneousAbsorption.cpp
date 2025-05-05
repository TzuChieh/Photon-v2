#include "Engine/Core/VolumeBehavior/VolumeOptics/HomogeneousAbsorption.h"
#include "Engine/Core/VolumeBehavior/Property/ConstantAbsorptionCoefficient.h"

#include <Common/assertion.h>

#include <cmath>

namespace ph
{

HomogeneousAbsorption::HomogeneousAbsorption()
	: HomogeneousAbsorption(math::Spectrum(0))
{}

HomogeneousAbsorption::HomogeneousAbsorption(const math::Spectrum& sigmaA)
	: VolumeOptics()
	, m_sigmaA(sigmaA)
{}

void HomogeneousAbsorption::genDistanceSample(
	const SurfaceHit& X,
	const math::Vector3R& L,
	const real maxDist,
	real* const out_dist,
	math::Spectrum* const out_pdfAppliedWeight) const
{
	PH_ASSERT(m_sigmaA && out_dist && out_pdfAppliedWeight);

	*out_dist = maxDist;

	// PDF = 1
	*out_pdfAppliedWeight = m_blockFunc->calcTransmittance(maxDist);
}

}// end namespace ph
