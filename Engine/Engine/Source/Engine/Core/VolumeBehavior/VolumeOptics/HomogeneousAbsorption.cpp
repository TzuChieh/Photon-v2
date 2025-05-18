#include "Engine/Core/VolumeBehavior/VolumeOptics/HomogeneousAbsorption.h"
#include "Engine/Core/VolumeBehavior/MediumDistanceSampleQuery.h"
#include "Engine/Core/VolumeBehavior/Property/TransmittanceFunction.h"

#include <Common/assertion.h>

namespace ph
{

HomogeneousAbsorption::HomogeneousAbsorption(
	const math::Spectrum& sigmaA,
	const TransmittanceFunction* transmittance)

	: VolumeOptics()

	, m_sigmaA(sigmaA)
	, m_transmittance(transmittance)
{
	PH_ASSERT(transmittance);
}

void HomogeneousAbsorption::genDistanceSample(
	const MediumDistanceSampleInput& in,
	SampleFlow& sampleFlow,
	MediumDistanceSampleOutput& out) const
{
	out.setDist(in.getMaxDist());

	// PDF = 1
	out.setPdfAppliedWeight(m_transmittance->eval(m_sigmaA * in.getMaxDist()));
}

}// end namespace ph
